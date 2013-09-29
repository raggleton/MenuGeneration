#include "l1menu/MenuRateOfflineScaling.h"

#include <stdexcept>
#include <map>
#include <fstream>
#include <iostream>
#include "l1menu/ITrigger.h"
#include "l1menu/ITriggerRate.h"
#include "l1menu/IMenuRate.h"
#include "l1menu/tools/tools.h"
#include "l1menu/tools/stringManipulation.h"

namespace // Use the unnamed namespace for things only used in this file
{

	/** @brief Proxy class that modifies the thresholds of another trigger according to a scaling
	 *
	 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
	 * @date 29/Sep/2013
	 */
	class TriggerProxy : public l1menu::ITrigger
	{
	public:
		TriggerProxy( const l1menu::ITrigger& unscaledTrigger )
			: pUnscaledTrigger_(&unscaledTrigger)
		{
		}
		void addScaling( const std::string& parameterName, float scale, float offset )
		{
			// Make sure this is a valid parameter by getting the value
			float unscaledValue=pUnscaledTrigger_->parameter(parameterName);

			// An exception will have been thrown before now if it wasn't a valid name
			scaledParameters_[parameterName]=unscaledValue*scale+offset;

			std::cout << "Scaling parameter " << parameterName << " from " << unscaledValue << " to " << scaledParameters_[parameterName] << " in trigger " << pUnscaledTrigger_->name() << std::endl;
		}
		TriggerProxy( TriggerProxy&& otherProxy ) noexcept
			: pUnscaledTrigger_(otherProxy.pUnscaledTrigger_),
			  scaledParameters_( std::move(otherProxy.scaledParameters_) )
		{
			// No operation
		}
		TriggerProxy& operator=( TriggerProxy&& otherProxy ) noexcept
		{
			pUnscaledTrigger_=otherProxy.pUnscaledTrigger_;
			scaledParameters_=std::move(otherProxy.scaledParameters_);
			return *this;
		}
		virtual ~TriggerProxy()
		{
			// No operation
		}
		virtual const std::string name() const
		{
			return pUnscaledTrigger_->name();
		}
		virtual unsigned int version() const
		{
			return pUnscaledTrigger_->version();
		}
		virtual const std::vector<std::string> parameterNames() const
		{
			return pUnscaledTrigger_->parameterNames();
		}
		virtual float& parameter( const std::string& parameterName )
		{
			throw std::logic_error("::TriggerProxy::parameter - You should never have a non const instance of this class");
		}
		virtual const float& parameter( const std::string& parameterName ) const
		{
			// First try and find the parameter in the list of scaled parameters
			auto iFindResult=scaledParameters_.find(parameterName);

			// If it wasn't found, return the unscaled parameter
			if( iFindResult==scaledParameters_.end() ) return pUnscaledTrigger_->parameter( parameterName );
			else return iFindResult->second;
		}
		virtual bool apply( const l1menu::L1TriggerDPGEvent& event ) const
		{
			throw std::runtime_error("::TriggerProxy::apply - You are trying to use a dummy trigger that has been created just to introduce a scaling");
		}
		virtual bool thresholdsAreCorrelated() const
		{
			return pUnscaledTrigger_->thresholdsAreCorrelated();
		}
	protected:
		const l1menu::ITrigger* pUnscaledTrigger_;
		std::map<std::string,float> scaledParameters_;
	};

	/** @brief Wraps a ITriggerRate instance and scales the thresholds.
	 *
	 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
	 * @date 29/Sep/2013
	 */
	class TriggerRateProxy : public l1menu::ITriggerRate
	{
	public:
		TriggerRateProxy( const l1menu::ITriggerRate& unscaledTriggerRate )
			: pUnscaledTriggerRate_(&unscaledTriggerRate),
			  triggerProxy_( unscaledTriggerRate.trigger() )
		{
			// No operation
		}
		TriggerRateProxy( TriggerRateProxy&& otherProxy ) noexcept
			: pUnscaledTriggerRate_(otherProxy.pUnscaledTriggerRate_),
			  triggerProxy_( std::move(otherProxy.triggerProxy_) )
		{
			// No operation
		}
		TriggerRateProxy& operator=( TriggerRateProxy&& otherProxy ) noexcept
		{
			pUnscaledTriggerRate_=otherProxy.pUnscaledTriggerRate_;
			triggerProxy_=std::move(otherProxy.triggerProxy_);
			return *this;
		}
		virtual ~TriggerRateProxy() {}
		virtual const l1menu::ITrigger& trigger() const { return triggerProxy_; }
		virtual float fraction() const { return pUnscaledTriggerRate_->fraction(); }
		virtual float fractionError() const { return pUnscaledTriggerRate_->fractionError(); }
		virtual float rate() const { return pUnscaledTriggerRate_->rate(); }
		virtual float rateError() const { return pUnscaledTriggerRate_->rateError(); }
		virtual float pureFraction() const { return pUnscaledTriggerRate_->pureFraction(); }
		virtual float pureFractionError() const { return pUnscaledTriggerRate_->pureFractionError(); }
		virtual float pureRate() const { return pUnscaledTriggerRate_->pureRate(); }
		virtual float pureRateError() const { return pUnscaledTriggerRate_->pureRateError(); }

		::TriggerProxy& triggerProxy() { return triggerProxy_; }
	protected:
		const l1menu::ITriggerRate* pUnscaledTriggerRate_;
		TriggerProxy triggerProxy_;
	};

	struct ThresholdScalings
	{
		float threshold1slope;
		float threshold1offset;
	};
}

//
// The pimple was implicitly declared, still need to declare it properly.
//
namespace l1menu
{
	class MenuRateOfflineScalingPrivateMembers
	{
	public:
		MenuRateOfflineScalingPrivateMembers( std::shared_ptr<const l1menu::IMenuRate> pUnscaledRate ) : pUnscaledMenuRate(pUnscaledRate) {}
		std::vector< ::TriggerRateProxy > triggerRates;
		std::vector<const l1menu::ITriggerRate*> triggerRateBaseClassPointers;
		std::shared_ptr<const l1menu::IMenuRate> pUnscaledMenuRate;
	};
}

l1menu::MenuRateOfflineScaling::MenuRateOfflineScaling( std::shared_ptr<const l1menu::IMenuRate> pUnscaledMenuRate, const std::string& offlineScalingFilename )
	: pImple_( new l1menu::MenuRateOfflineScalingPrivateMembers(pUnscaledMenuRate) )
{
	if( pUnscaledMenuRate==nullptr ) throw std::runtime_error( "Tried to create MenuRateOfflineScaling with a null pointer to the unscaled rate" );

	// Open the file that contains the scalings
	std::ifstream inputFile( offlineScalingFilename );
	if( !inputFile.is_open() ) throw std::runtime_error( "MenuRateOfflineScaling could not be constructed because the input file "+offlineScalingFilename+" could no be opened" );

	// This is a map that users the trigger name as a key. The value part is a vector
	// of the scalings for each of the thresholds, held as a pair with slope as "first"
	// and the offset as "second".
	std::map<std::string, std::vector<std::pair<float,float> > > triggerScalings;

	char buffer[200];
	while( inputFile.good() )
	{
		try
		{
			// Get one line at a time
			inputFile.getline( buffer, sizeof(buffer) );

			// split the line by whitespace into columns
			std::vector<std::string> tableColumns=l1menu::tools::splitByWhitespace( buffer );

			if( tableColumns.size()==1 && tableColumns[0].empty() ) continue; // Allow blank lines without giving a warning
			if( tableColumns.size()!=9 ) throw std::runtime_error( "MenuRateOfflineScaling could not be constructed because the input file does not have the correct number of columns" );

			// Read the trigger name and scalings from the file
			std::string triggerName=tableColumns[0];
			std::vector<std::pair<float,float> > scalings;

			using l1menu::tools::convertStringToFloat;

			scalings.push_back( std::make_pair( convertStringToFloat(tableColumns[1]), convertStringToFloat(tableColumns[2]) ) );
			scalings.push_back( std::make_pair( convertStringToFloat(tableColumns[3]), convertStringToFloat(tableColumns[4]) ) );
			scalings.push_back( std::make_pair( convertStringToFloat(tableColumns[5]), convertStringToFloat(tableColumns[6]) ) );
			scalings.push_back( std::make_pair( convertStringToFloat(tableColumns[7]), convertStringToFloat(tableColumns[8]) ) );

			// Then store them in the map
			triggerScalings[triggerName]=scalings;

		} // end of try block
		catch( std::runtime_error& exception )
		{
			std::cerr << "Some error occured while processing the line \"" << buffer << "\":" << exception.what() << std::endl;
		}
	}
	inputFile.close();

	// I should now have all of the scalings in the map triggerScalings with the
	// trigger name as the key. I now need to run over all of the TriggerScalings
	// in the menu scaling and create proxy objects for them, adding the scalings
	// where required.
	for( const auto pTriggerScaling : pImple_->pUnscaledMenuRate->triggerRates() )
	{
		pImple_->triggerRates.push_back( ::TriggerRateProxy(*pTriggerScaling) );

		// See if this trigger needs to have its parameters changed
		const auto& iFindResult=triggerScalings.find( pTriggerScaling->trigger().name() );
		if( iFindResult!=triggerScalings.end() )
		{
			std::vector<std::pair<float,float> >& scalings=iFindResult->second;
			::TriggerProxy& trigger=pImple_->triggerRates.back().triggerProxy();

			std::vector<std::string> thresholdNames=l1menu::tools::getThresholdNames( trigger );
			for( size_t index=0; index<thresholdNames.size(); ++index )
			{
				trigger.addScaling( thresholdNames[index], scalings[index].first, scalings[index].second );
			}
		}
	}

}

l1menu::MenuRateOfflineScaling::~MenuRateOfflineScaling()
{

}

float l1menu::MenuRateOfflineScaling::totalFraction() const
{
	return pImple_->pUnscaledMenuRate->totalFraction();
}

float l1menu::MenuRateOfflineScaling::totalFractionError() const
{
	return pImple_->pUnscaledMenuRate->totalFractionError();
}

float l1menu::MenuRateOfflineScaling::totalRate() const
{
	return pImple_->pUnscaledMenuRate->totalRate();
}

float l1menu::MenuRateOfflineScaling::totalRateError() const
{
	return pImple_->pUnscaledMenuRate->totalRateError();
}

const std::vector<const l1menu::ITriggerRate*>& l1menu::MenuRateOfflineScaling::triggerRates() const
{
	// Refill the vector of base class pointers in case the triggerRates vector memory has
	// been moved.
	pImple_->triggerRateBaseClassPointers.clear();
	for( const auto& triggerRate : pImple_->triggerRates )
	{
		pImple_->triggerRateBaseClassPointers.push_back( &triggerRate );
	}

	return pImple_->triggerRateBaseClassPointers;
}
