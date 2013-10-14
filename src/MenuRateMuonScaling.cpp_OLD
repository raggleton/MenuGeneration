#include "l1menu/MenuRateMuonScaling.h"

#include <stdexcept>
#include <iostream>
#include <cmath>
#include <l1menu/ITrigger.h>
#include <l1menu/ITriggerRate.h>
#include <l1menu/MenuFitter.h>
#include <l1menu/TriggerRatePlot.h>
#include <l1menu/tools/tools.h>
#include <TH1.h>
#include <TFile.h>


namespace // Use the unnamed namespace for things only used in this file
{
	/** @brief Function copied straight from TriggerRatePlot::findThreshold. Once I've got these results I'll put it somewhere sensible.
	 *
	 */
	float findThreshold( float targetRate, TH1* pRateHistogram )
	{
		//
		// Loop over all of the bins in the plot and find the first one
		// that is less than the requested rate.
		//
		int binNumber;
		for( binNumber=1; binNumber<pRateHistogram->GetNbinsX(); ++binNumber )
		{
			if( pRateHistogram->GetBinContent(binNumber)<targetRate ) break;
		}

		// If the search got to the end, put it one back so that when I fill
		// the binNumberForLinearFit vector below it includes the last four
		// bins for the linear fit.
		if( binNumber==pRateHistogram->GetNbinsX() ) binNumber=pRateHistogram->GetNbinsX()-1;
		// Likewise make sure if I'm at the start that when I fill the vector
		// the bins it uses actually exist.
		if( binNumber<3 ) binNumber=3;

		// I now have the bin number of the bin after the point I'm looking
		// for. Now do a linear fit to interpolate between the bins using the
		// two bins for the point and the two after.
		std::vector<int> binNumberForLinearFit;
		binNumberForLinearFit.push_back( binNumber-2 );
		binNumberForLinearFit.push_back( binNumber-1 );
		binNumberForLinearFit.push_back( binNumber );
		binNumberForLinearFit.push_back( binNumber+1 );

		std::vector< std::pair<float,float> > dataPoints;
		for( auto& number : binNumberForLinearFit )
		{
			// Make sure all of of the bin numbers are valid
			if( number<1 ) number=1;
			else if( number>pRateHistogram->GetNbinsX() ) number=pRateHistogram->GetNbinsX();
			dataPoints.push_back( std::make_pair( pRateHistogram->GetBinLowEdge(number), pRateHistogram->GetBinContent(number) ));
		}

		// Now do a simple linear fit on the data points
		std::pair<float,float> slopeAndIntercept=l1menu::tools::simpleLinearFit( dataPoints );
		float slope=slopeAndIntercept.first;
		float intercept=slopeAndIntercept.second;

		if( std::fabs(slope)<std::pow(10,-4) ) // see if the slope==0 within a little tolerance
		{
			// Some of the triggers, e.g. muons, can only have the threshold set in discrete steps. So the rate plot
			// comes out looking as though it has steps in it. All of the bins for the linear fit could be on the same
			// step (I think this will only happen if it's the bins at the high threshold end, but anyway). In this
			// case I'll just come back along the plot and return the value for the lowest bin, i.e. the lowest
			// threshold for this particular "step".
			while( binNumber>1 && pRateHistogram->GetBinContent(binNumber)==pRateHistogram->GetBinContent(binNumber-1) ) --binNumber;
			return pRateHistogram->GetBinLowEdge(binNumber);
		}
		else
		{
			float newThreshold=(targetRate-intercept)/slope;
			if( newThreshold<0 ) return 0; // Apply some sanity checks
			// Do I want to extrapolate out past where the histogram goes? Not sure. I won't for the time being.
			else if( newThreshold>pRateHistogram->GetXaxis()->GetXmax() ) return pRateHistogram->GetXaxis()->GetXmax();
			else return newThreshold;
		}
	}

	/** @brief Proxy class that modifies the thresholds of another trigger according to a scaling
	 *
	 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
	 * @date 29/Sep/2013
	 */
	class TriggerProxy : public l1menu::ITrigger
	{
	public:
		TriggerProxy( const l1menu::ITrigger& unscaledTrigger, float scale=1 )
			: pUnscaledTrigger_(&unscaledTrigger),
			  thresholdNames_( l1menu::tools::getThresholdNames( *pUnscaledTrigger_ ) )
		{
			setScaling( scale );
		}
		void setScaling( float scale )
		{
			scaledThresholdValues_.clear();

			// Record the values of what all the thresholds are once the scaling is applied
			for( const auto& thresholdName : thresholdNames_ )
			{
				scaledThresholdValues_.push_back( scale*pUnscaledTrigger_->parameter(thresholdName) );
			}
		}
		/// @brief Calculates a scaling so that the primary threshold will then equal the provided value
		void setPrimaryThresholdAfterScaling( float newPrimaryThreshold )
		{
			setScaling( newPrimaryThreshold/pUnscaledTrigger_->parameter(thresholdNames_.front()) );
		}
		TriggerProxy( TriggerProxy&& otherProxy ) noexcept
			: pUnscaledTrigger_(otherProxy.pUnscaledTrigger_),
			  thresholdNames_( std::move(otherProxy.thresholdNames_) ),
			  scaledThresholdValues_( std::move(otherProxy.scaledThresholdValues_) )
		{
			// No operation
		}
		TriggerProxy& operator=( TriggerProxy&& otherProxy ) noexcept
		{
			pUnscaledTrigger_=otherProxy.pUnscaledTrigger_;
			thresholdNames_=std::move(otherProxy.thresholdNames_);
			scaledThresholdValues_=std::move(otherProxy.scaledThresholdValues_);
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
			// See if the parameter is one of the thresholds
			for( size_t index=0; index<thresholdNames_.size(); ++index )
			{
				if( thresholdNames_[index]==parameterName ) return scaledThresholdValues_[index];
			}

			// If control flow has got to this point then the parameter is not
			// one of the thresholds, and I can return the value straight from
			// the real trigger.
			return pUnscaledTrigger_->parameter( parameterName );
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
		std::vector<std::string> thresholdNames_;
		std::vector<float> scaledThresholdValues_;
	};

	/** @brief Wraps a ITriggerRate instance and scales the thresholds.
	 *
	 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
	 * @date 29/Sep/2013
	 */
	class TriggerRateProxy : public l1menu::ITriggerRate
	{
	public:
		TriggerRateProxy( const l1menu::ITriggerRate& unscaledTriggerRate, std::unique_ptr<TH1> pScaledRatePlot )
			: pUnscaledTriggerRate_(&unscaledTriggerRate),
			  triggerProxy_( unscaledTriggerRate.trigger() ),
			  pScaledRatePlot_( std::move(pScaledRatePlot) )
		{
			// Figure out what the scaling should be
			float unscaledRate=pUnscaledTriggerRate_->rate();

			// See what threshold this corresponds to on the scaled rate plot
			float scaledThreshold=::findThreshold( unscaledRate, pScaledRatePlot_.get() );

			triggerProxy_.setPrimaryThresholdAfterScaling( scaledThreshold );
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
	protected:
		const l1menu::ITriggerRate* pUnscaledTriggerRate_;
		TriggerProxy triggerProxy_;
		std::unique_ptr<TH1> pScaledRatePlot_;
	};

}

//
// The pimple was implicitly declared, still need to declare it properly.
//
namespace l1menu
{
	class MenuRateMuonScalingPrivateMembers
	{
	public:
		MenuRateMuonScalingPrivateMembers( std::shared_ptr<const l1menu::IMenuRate> pUnscaledRate ) : pUnscaledMenuRate(pUnscaledRate) {}
		std::vector< ::TriggerRateProxy > triggerRates;
		std::vector<const l1menu::ITriggerRate*> triggerRateBaseClassPointers;
		std::shared_ptr<const l1menu::IMenuRate> pUnscaledMenuRate;
		std::unique_ptr<TH1> pMuonScalePtPlot;
		std::unique_ptr<TH1> pMuonScaleIsolationPlot;
	};
}

l1menu::MenuRateMuonScaling::MenuRateMuonScaling( std::shared_ptr<const l1menu::IMenuRate> pUnscaledMenuRate, const std::string& muonScalingFilename, const l1menu::MenuFitter& fitter )
	: pImple_( new l1menu::MenuRateMuonScalingPrivateMembers(pUnscaledMenuRate) )
{
	{ // Block to limit scope of temporary variables
		std::unique_ptr<TFile> pMuonScalingFile( TFile::Open(muonScalingFilename.c_str()) );
		if( pMuonScalingFile==nullptr ) throw std::runtime_error( "Couldn't create MenuRateMuonScaling because couldn't open TFile "+muonScalingFilename );

		pImple_->pMuonScalePtPlot.reset( static_cast<TH1*>( pMuonScalingFile->Get("muonPtScale")->Clone() ) );
		pImple_->pMuonScalePtPlot->SetDirectory( nullptr ); // Make sure it's only held in memory. Might be the default for clones but not sure.
		if( pImple_->pMuonScalePtPlot==nullptr ) throw std::runtime_error( "Couldn't create MenuRateMuonScaling because couldn't get the TH1 \"muonPtScale\" from the  TFile" );

		pImple_->pMuonScaleIsolationPlot.reset( static_cast<TH1*>( pMuonScalingFile->Get("muonIsoScale")->Clone() ) );
		pImple_->pMuonScaleIsolationPlot->SetDirectory( nullptr ); // Make sure it's only held in memory.
		if( pImple_->pMuonScaleIsolationPlot==nullptr ) throw std::runtime_error( "Couldn't create MenuRateMuonScaling because couldn't get the TH1 \"muonIsoScale\" from the  TFile" );
	}

	//
	// Now run through each of the triggers and see which ones are for muons.
	// If they are, create a proxy object that will change
	//

	//
	// Running out of time to get results. Just going to hardcode a load of shit for now and
	// do all scaling in this class. I'll sort it all out after the TPSWG meeting.
	//
	std::string filename="/home/xtaldaq/CMSSWReleases/CMSSW_5_3_4/src/v19Results/L1RateHist_8TeV45PU_25nsMCMC_FallbackThr1_rates.root";
	std::unique_ptr<TFile> pMCScalingFile( TFile::Open(filename.c_str()) );
	if( pMCScalingFile==nullptr ) throw std::runtime_error( "Couldn't create MenuRateMuonScaling because couldn't open TFile "+filename );

	filename="/home/xtaldaq/CMSSWReleases/CMSSW_5_3_4/src/v19Results/L1RateHist_8TeV45PU_25nsDataMC_FallbackThr1_rates.root";
	std::unique_ptr<TFile> pDataScalingFile( TFile::Open(filename.c_str()) );
	if( pDataScalingFile==nullptr ) throw std::runtime_error( "Couldn't create MenuRateMuonScaling because couldn't open TFile "+filename );

	size_t triggerNumber=0;
	for( const auto pTriggerRate : pImple_->pUnscaledMenuRate->triggerRates() )
	{
		std::string fullName=pTriggerRate->trigger().name();
		std::string oldCodeHistogramName="h_"+fullName.substr(3)+"_byThreshold";
		std::cout << "attempting to load histogram " << oldCodeHistogramName << std::endl;
		TH1* pMCHistogram=static_cast<TH1*>( pMCScalingFile->Get( oldCodeHistogramName.c_str() ) );
		TH1* pDataHistogram=static_cast<TH1*>( pDataScalingFile->Get( oldCodeHistogramName.c_str() ) );
		if( pMCHistogram==nullptr || pDataHistogram==nullptr ) throw std::runtime_error("Couldn't load histogram "+oldCodeHistogramName);

		const TH1* pOriginalRatePlot=fitter.triggerRatePlot(triggerNumber).getPlot();
		if( pOriginalRatePlot==nullptr) throw std::runtime_error("Couldn't get the rate plot for "+fullName);

		std::unique_ptr<TH1> pRatePlotClone( static_cast<TH1*>(pOriginalRatePlot->Clone()) );
		pRatePlotClone->SetDirectory(nullptr);

		pRatePlotClone->Multiply( pDataHistogram );
		pRatePlotClone->Divide( pMCHistogram );

		// Brian's original comment: Scaling for Muon System Upgrades  (Problem: Can't easily scale
		// the EG_Mu trigger because x-axis is EG Et not muon Pt...need to fix)
		if( fullName.find("Mu")!=std::string::npos && fullName.find("EG_Mu")==std::string::npos )
		{
			std::cout << "Applying scale factor for muon pt assignment for " << fullName << std::endl;
			// I changed the binning on a lot of the histograms, but haven't done so for the muon scaling
			// plots. As a quick thing to get results for the TPSWG meeting I'll multiply each bin by hand.
			// The root TH1::Rebin method doesn't work very well unless the bin edges line up.
			// The pMuonScalePtPlot plot is quite discretised anyway, and the binning is almost similar. So
			// I don't think it'll make much difference.
			for( int bin=1; bin<=pRatePlotClone->GetNbinsX(); ++bin )
			{
				// Using the bin low edge because that's the value I use to see if a bin has
				// passed a trigger.
				int scaleBin=pImple_->pMuonScalePtPlot->FindBin( pRatePlotClone->GetBinLowEdge( bin ) );
				float scaleFactor=pImple_->pMuonScalePtPlot->GetBinContent(scaleBin);
				pRatePlotClone->SetBinContent( bin, pRatePlotClone->GetBinContent(bin)*scaleFactor );
			}
		}

		if( fullName.find("IsoMu")!=std::string::npos || fullName.find("isoMu")!=std::string::npos )
		{
			std::cout << "Applying scale factor for muon isolation for " << fullName << std::endl;
			// See note above for pt assignment scale factor
			for( int bin=1; bin<=pRatePlotClone->GetNbinsX(); ++bin )
			{
				int scaleBin=pImple_->pMuonScaleIsolationPlot->FindBin( pRatePlotClone->GetBinLowEdge( bin ) );
				float scaleFactor=pImple_->pMuonScaleIsolationPlot->GetBinContent(scaleBin);
				pRatePlotClone->SetBinContent( bin, pRatePlotClone->GetBinContent(bin)*scaleFactor );
			}
		}

		pImple_->triggerRates.push_back( std::move( ::TriggerRateProxy( *pTriggerRate, std::move(pRatePlotClone) ) ) );

		++triggerNumber;
	}
}

l1menu::MenuRateMuonScaling::~MenuRateMuonScaling()
{
	// No operation
}

// The methods required by the IMenuRate interface
float l1menu::MenuRateMuonScaling::totalFraction() const
{
	return pImple_->pUnscaledMenuRate->totalFraction();
}

float l1menu::MenuRateMuonScaling::totalFractionError() const
{
	return pImple_->pUnscaledMenuRate->totalFractionError();
}

float l1menu::MenuRateMuonScaling::totalRate() const
{
	return pImple_->pUnscaledMenuRate->totalRate();
}

float l1menu::MenuRateMuonScaling::totalRateError() const
{
	return pImple_->pUnscaledMenuRate->totalRateError();
}

const std::vector<const l1menu::ITriggerRate*>& l1menu::MenuRateMuonScaling::triggerRates() const
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

