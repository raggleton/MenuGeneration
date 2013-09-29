#include "l1menu/scalings/MuonAndMCScaling.h"

#include <stdexcept>
#include <iostream>
#include "l1menu/TriggerRatePlot.h"
#include "l1menu/MenuRatePlots.h"
#include "l1menu/IMenuRate.h"
#include "l1menu/ITrigger.h"
#include <TH1.h>
#include <TFile.h>

//
// Pimple class was implicitly declared, need to explicitly declare and define it
//
namespace l1menu
{
	namespace scalings
	{
		class MuonAndMCScalingPrivateMembers
		{
		public:
			std::unique_ptr<TH1> pMuonScalePtPlot;
			std::unique_ptr<TH1> pMuonScaleIsolationPlot;
			std::unique_ptr<TFile> pMCScalingFile;
			std::unique_ptr<TFile> pDataScalingFile;
			void performScaling( TH1* pHistogramToScale, const std::string& triggerName );
		};

	} // end of namespace scalings
} // end of namespace l1menu

l1menu::scalings::MuonAndMCScaling::MuonAndMCScaling( const std::string& muonScalingFilename, const std::string& monteCarloRatesFilename, const std::string& dataRatesFilename )
	: pImple_( new MuonAndMCScalingPrivateMembers )
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
	std::unique_ptr<TFile> pMuonScalingFile( TFile::Open(muonScalingFilename.c_str()) );
	if( pMuonScalingFile==nullptr ) throw std::runtime_error( "Couldn't create MuonAndMCScaling because couldn't open TFile "+muonScalingFilename );

	std::unique_ptr<TFile> pMCScalingFile( TFile::Open(monteCarloRatesFilename.c_str()) );
	if( pMCScalingFile==nullptr ) throw std::runtime_error( "Couldn't create MuonAndMCScaling because couldn't open TFile "+monteCarloRatesFilename );

	std::unique_ptr<TFile> pDataScalingFile( TFile::Open(dataRatesFilename.c_str()) );
	if( pDataScalingFile==nullptr ) throw std::runtime_error( "Couldn't create MuonAndMCScaling because couldn't open TFile "+dataRatesFilename );


	//
	// Now the files opened successfully, try and get all of the required plots
	//
	pImple_->pMuonScalePtPlot.reset( static_cast<TH1*>( pMuonScalingFile->Get("muonPtScale")->Clone() ) );
	if( pImple_->pMuonScalePtPlot==nullptr ) throw std::runtime_error( "Couldn't create MuonAndMCScaling because couldn't get the TH1 \"muonPtScale\" from the TFile "+muonScalingFilename );
	pImple_->pMuonScalePtPlot->SetDirectory( nullptr ); // Make sure it's only held in memory. Might be the default for clones but not sure.

	pImple_->pMuonScaleIsolationPlot.reset( static_cast<TH1*>( pMuonScalingFile->Get("muonIsoScale")->Clone() ) );
	if( pImple_->pMuonScaleIsolationPlot==nullptr ) throw std::runtime_error( "Couldn't create MuonAndMCScaling because couldn't get the TH1 \"muonIsoScale\" from the TFile "+muonScalingFilename );
	pImple_->pMuonScaleIsolationPlot->SetDirectory( nullptr ); // Make sure it's only held in memory.

	pMuonScalingFile->Close(); // Taken copies of the muon plots so I can close the file now.

	//
	// I'm a bit short of time before tomorrow's meeting so I'll keep the file open and get the plots as I need them.
	// I'd have preferred to take copies.
	//
	// TODO - take copies of all the plots
	pImple_->pMCScalingFile=std::move(pMCScalingFile);
	pImple_->pDataScalingFile=std::move(pDataScalingFile);
}

l1menu::scalings::MuonAndMCScaling::~MuonAndMCScaling()
{
	// No operation
}

std::string l1menu::scalings::MuonAndMCScaling::briefDescription()
{
	return "Scales for muon pT assignment and isolation, as well as differences between data and MonteCarlo.";
}

std::string l1menu::scalings::MuonAndMCScaling::detailedDescription()
{
	return "I'll write this later.";
}

std::unique_ptr<l1menu::TriggerRatePlot> l1menu::scalings::MuonAndMCScaling::scaleTriggerRatePlot( const l1menu::TriggerRatePlot& unscaledPlot )
{
	// Take a copy and work on that
	std::unique_ptr<l1menu::TriggerRatePlot> pReturnValue( new l1menu::TriggerRatePlot( unscaledPlot ) );

	TH1* pRawPlot=pReturnValue->getPlot();
	// First make sure it's held in memory. The user can change that later if they want.
	pRawPlot->SetDirectory( nullptr );
	// Delegate to a private method to do the work
	pImple_->performScaling( pRawPlot, pReturnValue->getTrigger().name() );

	return pReturnValue;
}

std::unique_ptr<l1menu::MenuRatePlots> l1menu::scalings::MuonAndMCScaling::scaleMenuRatePlots( const l1menu::MenuRatePlots& unscaledPlots )
{
	// First take a copy of the input and modify that directly
	std::unique_ptr<l1menu::MenuRatePlots> pReturnValue( new l1menu::MenuRatePlots( unscaledPlots ) );

	// Loop over all of the plots and do the business
	for( auto& triggerRatePlot : pReturnValue->triggerRatePlots() )
	{
		TH1* pRawPlot=triggerRatePlot.getPlot();
		// First make sure all of them are only held in memory. The user can change that
		// later if they want.
		pRawPlot->SetDirectory( nullptr );
		// Delegate to a private method to do the work
		pImple_->performScaling( pRawPlot, triggerRatePlot.getTrigger().name() );
	}

	return pReturnValue;
}

std::shared_ptr<l1menu::IMenuRate> l1menu::scalings::MuonAndMCScaling::scaleMenuRate( const l1menu::IMenuRate& unscaledMenuRate )
{
	throw std::logic_error("l1menu::scalings::MuonAndMCScaling::scaleMenuRate not implemented yet");
}

void l1menu::scalings::MuonAndMCScalingPrivateMembers::performScaling( TH1* pHistogramToScale, const std::string& triggerName )
{
	std::string oldCodeHistogramName="h_"+triggerName.substr(3)+"_byThreshold";
	std::cout << "attempting to load histogram " << oldCodeHistogramName << std::endl;
	TH1* pMCHistogram=static_cast<TH1*>( pMCScalingFile->Get( oldCodeHistogramName.c_str() ) );
	TH1* pDataHistogram=static_cast<TH1*>( pDataScalingFile->Get( oldCodeHistogramName.c_str() ) );
	if( pMCHistogram==nullptr || pDataHistogram==nullptr ) throw std::runtime_error("Couldn't load histogram "+oldCodeHistogramName);

	pHistogramToScale->Multiply( pDataHistogram );
	pHistogramToScale->Divide( pMCHistogram );

	// Brian's original comment: Scaling for Muon System Upgrades  (Problem: Can't easily scale
	// the EG_Mu trigger because x-axis is EG Et not muon Pt...need to fix)
	if( triggerName.find("Mu")!=std::string::npos && triggerName.find("EG_Mu")==std::string::npos )
	{
		std::cout << "Applying scale factor for muon pt assignment for " << triggerName << std::endl;
		// I changed the binning on a lot of the histograms, but haven't done so for the muon scaling
		// plots. As a quick thing to get results for the TPSWG meeting I'll multiply each bin by hand.
		// The root TH1::Rebin method doesn't work very well unless the bin edges line up.
		// The pMuonScalePtPlot plot is quite discretised anyway, and the binning is almost similar. So
		// I don't think it'll make much difference.
		for( int bin=1; bin<=pHistogramToScale->GetNbinsX(); ++bin )
		{
			// Using the bin low edge because that's the value I use to see if a bin has
			// passed a trigger.
			int scaleBin=pMuonScalePtPlot->FindBin( pHistogramToScale->GetBinLowEdge( bin ) );
			float scaleFactor=pMuonScalePtPlot->GetBinContent(scaleBin);
			pHistogramToScale->SetBinContent( bin, pHistogramToScale->GetBinContent(bin)*scaleFactor );
		}
	}

	if( triggerName.find("IsoMu")!=std::string::npos || triggerName.find("isoMu")!=std::string::npos )
	{
		std::cout << "Applying scale factor for muon isolation for " << triggerName << std::endl;
		// See note above for pt assignment scale factor
		for( int bin=1; bin<=pHistogramToScale->GetNbinsX(); ++bin )
		{
			int scaleBin=pMuonScaleIsolationPlot->FindBin( pHistogramToScale->GetBinLowEdge( bin ) );
			float scaleFactor=pMuonScaleIsolationPlot->GetBinContent(scaleBin);
			pHistogramToScale->SetBinContent( bin, pHistogramToScale->GetBinContent(bin)*scaleFactor );
		}
	}

}
