#include "l1menu/TriggerTable.h"

#include "l1menu/ITrigger.h"

#include <sstream>
#include <stdexcept>

//
// Declare the pimple class
//
namespace l1menu
{
	class TriggerTablePrivateMembers
	{
	public:
		struct TriggerRegistryEntry
		{
			l1menu::TriggerTable::TriggerDetails details;
			std::unique_ptr<l1menu::ITrigger> (*creationFunctionPointer)();
		};
		struct SuggestedBinning
		{
			unsigned int numberOfBins;
			float lowerEdge;
			float upperEdge;
		};
		std::vector<TriggerRegistryEntry> registeredTriggers;
		std::map<std::string,std::map<std::string,SuggestedBinning> > suggestedBinning_;
		const SuggestedBinning& getSuggestedBinning( const std::string& triggerName, const std::string& parameterName );
	};

} // end of namespace l1menu

const l1menu::TriggerTablePrivateMembers::SuggestedBinning& l1menu::TriggerTablePrivateMembers::getSuggestedBinning( const std::string& triggerName, const std::string& parameterName )
{
	const auto& iTriggerFindResult=suggestedBinning_.find(triggerName);
	if( iTriggerFindResult==suggestedBinning_.end() )
	{
		throw std::runtime_error( "Trigger \""+triggerName+"\" has no suggested binning registered." );
	}

	const auto& iParameterFindResult=iTriggerFindResult->second.find(parameterName);
	if( iParameterFindResult==iTriggerFindResult->second.end() )
	{
		throw std::runtime_error( "Trigger \""+triggerName+"\" has no suggested binning registered for parameter \""+parameterName+"\"." );
	}

	return iParameterFindResult->second;
}

l1menu::TriggerTable& l1menu::TriggerTable::instance()
{
	static TriggerTable onlyInstance;
	return onlyInstance;
}

l1menu::TriggerTable::TriggerTable() : pImple_( new l1menu::TriggerTablePrivateMembers )
{
	// No operation. Only declared so that it can be declared private.
}

l1menu::TriggerTable::~TriggerTable()
{
	// No operation. Only declared so that it can be declared private.
}

bool l1menu::TriggerTable::TriggerDetails::operator==( const l1menu::TriggerTable::TriggerDetails& otherTriggerDetails ) const
{
	return name==otherTriggerDetails.name && version==otherTriggerDetails.version;
}

std::unique_ptr<l1menu::ITrigger> l1menu::TriggerTable::getTrigger( const std::string& name ) const
{
//	std::cout << "Looking for latest version of " << name << std::endl;

	std::unique_ptr<l1menu::ITrigger> returnValue;
	unsigned int highestVersionNumber=0; // The highest version of the trigger encountered in the list so far

	for( std::vector<TriggerTablePrivateMembers::TriggerRegistryEntry>::const_iterator iRegistryEntry=pImple_->registeredTriggers.begin(); iRegistryEntry!=pImple_->registeredTriggers.end(); ++iRegistryEntry )
	{
		if( iRegistryEntry->details.name==name && iRegistryEntry->details.version>=highestVersionNumber )
		{
			returnValue=(*iRegistryEntry->creationFunctionPointer)();
			highestVersionNumber=iRegistryEntry->details.version;
		}
	}

	return returnValue;
}

std::unique_ptr<l1menu::ITrigger> l1menu::TriggerTable::getTrigger( const std::string& name, unsigned int version ) const
{
	TriggerDetails requestedTriggerDetails{ name, version };

	// Delegate to the other overload
	return getTrigger( requestedTriggerDetails );
}

std::unique_ptr<l1menu::ITrigger> l1menu::TriggerTable::getTrigger( const TriggerDetails& details ) const
{
//	std::cout << "Looking for version " << details.version << " of " << details.name << std::endl;

	for( std::vector<TriggerTablePrivateMembers::TriggerRegistryEntry>::const_iterator iRegistryEntry=pImple_->registeredTriggers.begin(); iRegistryEntry!=pImple_->registeredTriggers.end(); ++iRegistryEntry )
	{
		if( iRegistryEntry->details==details )
		{
			return (*iRegistryEntry->creationFunctionPointer)();
		}
	}

	// If program flow has reached this point then there are no triggers registered that
	// match the criteria. Return an empty pointer.
	return std::unique_ptr<l1menu::ITrigger>();
}

std::unique_ptr<l1menu::ITrigger> l1menu::TriggerTable::copyTrigger( const l1menu::ITrigger& triggerToCopy ) const
{
	// First create a trigger with the matching name and version
	std::unique_ptr<l1menu::ITrigger> newTrigger=getTrigger( triggerToCopy.name(), triggerToCopy.version() );

	if( newTrigger.get()==NULL ) throw std::runtime_error( "Unable to copy trigger "+triggerToCopy.name() );

	//
	// Now copy all of the parameters over.
	//
	// Get the parameter names
	std::vector<std::string> parameterNames=triggerToCopy.parameterNames();
	// Then run through and copy the value of each one
	for( std::vector<std::string>::const_iterator iName=parameterNames.begin(); iName!=parameterNames.end(); ++iName )
	{
		newTrigger->parameter(*iName)=triggerToCopy.parameter(*iName);
	}

	return newTrigger;
}

std::vector<l1menu::TriggerTable::TriggerDetails> l1menu::TriggerTable::listTriggers() const
{
	std::vector<TriggerDetails> returnValue;

	// Copy the relevant parts from the registered triggers into the return value
	for( std::vector<TriggerTablePrivateMembers::TriggerRegistryEntry>::const_iterator iRegistryEntry=pImple_->registeredTriggers.begin(); iRegistryEntry!=pImple_->registeredTriggers.end(); ++iRegistryEntry )
	{
		returnValue.push_back( iRegistryEntry->details );
	}

	return returnValue;
}

void l1menu::TriggerTable::registerTrigger( const std::string& name, unsigned int version, std::unique_ptr<l1menu::ITrigger> (*creationFunctionPointer)() )
{
	TriggerDetails newTriggerDetails{ name, version };

	// First make sure there is not a trigger with the same name and version already registered
	for( std::vector<TriggerTablePrivateMembers::TriggerRegistryEntry>::const_iterator iRegistryEntry=pImple_->registeredTriggers.begin(); iRegistryEntry!=pImple_->registeredTriggers.end(); ++iRegistryEntry )
	{
		if( iRegistryEntry->details==newTriggerDetails )
		{
			std::stringstream errorMessage;
			errorMessage << "A trigger called \"" << newTriggerDetails.name << "\" with version " << newTriggerDetails.version << " has already been registered in the trigger table.";
			throw std::logic_error( errorMessage.str() );
		}
	}

	// If program flow has reached this point then there are no triggers with the same name
	// and version already registered, so it's okay to add the trigger as requested.
	pImple_->registeredTriggers.push_back( TriggerTablePrivateMembers::TriggerRegistryEntry{newTriggerDetails,creationFunctionPointer} );
}

void l1menu::TriggerTable::registerSuggestedBinning( const std::string& triggerName, const std::string& parameterName, unsigned int numberOfBins, float lowerEdge, float upperEdge )
{
	pImple_->suggestedBinning_[triggerName][parameterName]={ numberOfBins, lowerEdge, upperEdge };
}

unsigned int l1menu::TriggerTable::getSuggestedNumberOfBins( const std::string& triggerName, const std::string& parameterName ) const
{
	try
	{
		return pImple_->getSuggestedBinning( triggerName, parameterName ).numberOfBins;
	}
	catch( std::runtime_error& error )
	{
		throw std::runtime_error( std::string("TriggerTable::getSuggestedNumberOfBins - ")+error.what() );
	}
}

float l1menu::TriggerTable::getSuggestedLowerEdge( const std::string& triggerName, const std::string& parameterName ) const
{
	try
	{
		return pImple_->getSuggestedBinning( triggerName, parameterName ).lowerEdge;
	}
	catch( std::runtime_error& error )
	{
		throw std::runtime_error( std::string("TriggerTable::getSuggestedLowerEdge - ")+error.what() );
	}
}

float l1menu::TriggerTable::getSuggestedUpperEdge( const std::string& triggerName, const std::string& parameterName ) const
{
	try
	{
		return pImple_->getSuggestedBinning( triggerName, parameterName ).upperEdge;
	}
	catch( std::runtime_error& error )
	{
		throw std::runtime_error( std::string("TriggerTable::getSuggestedUpperEdge - ")+error.what() );
	}
}
