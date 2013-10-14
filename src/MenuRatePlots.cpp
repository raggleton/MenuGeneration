#include "l1menu/MenuRatePlots.h"

#include <sstream>
#include <algorithm>
#include "l1menu/ITrigger.h"
#include "l1menu/TriggerMenu.h"
#include "l1menu/TriggerRatePlot.h"
#include "l1menu/tools/miscellaneous.h"
#include <TH1F.h>
#include <TDirectory.h>
#include <TKey.h>
#include <iostream>

l1menu::MenuRatePlots::MenuRatePlots( const l1menu::TriggerMenu& triggerMenu, TDirectory* pDirectory )
{
	// Before making any histograms make sure errors are done properly
	TH1::SetDefaultSumw2();

	// This is always useful
	const l1menu::TriggerTable& triggerTable=l1menu::TriggerTable::instance();

	std::stringstream stringConverter; // Used to convert numbers into to strings
	std::vector<std::string> usedNames; // A list of all the names I've already used for plots, so that I can make sure all names are unique

	// Loop over each of the triggers in the menu, book a histogram for it and then create
	// a l1menu::TriggerRate plot for it.
	for( size_t triggerNumber=0; triggerNumber<triggerMenu.numberOfTriggers(); ++triggerNumber )
	{
		std::unique_ptr<l1menu::ITrigger> pTrigger=triggerMenu.getTriggerCopy(triggerNumber);
		// Figure out the parameter names of all the possible thresholds.
		const std::vector<std::string> thresholdNames=l1menu::tools::getThresholdNames(*pTrigger);

		// I could have more than one trigger of the same type with different parameters, so I need to
		// figure out different names for each of the plots. Keep appending an increasing integer to the
		// end of the trigger name until I find an unused name.
		std::string triggerNameForPlot=pTrigger->name();
		for( size_t index=2; true; ++index )
		{
			if( std::find( usedNames.begin(), usedNames.end(), triggerNameForPlot )==usedNames.end() )
			{
				usedNames.push_back( triggerNameForPlot );
				break;
			}
			stringConverter.str("");
			stringConverter << pTrigger->name() << index;
			triggerNameForPlot=stringConverter.str();
		}

		//
		// If there is more than one threshold add a plot where all of the thresholds are scaled together.
		//
		if( thresholdNames.size()>1 )
		{
			const std::string& mainThreshold=thresholdNames.front();
			unsigned int numberOfBins=100;
			float lowerEdge=0;
			float upperEdge=100;
			try
			{
				numberOfBins=triggerTable.getSuggestedNumberOfBins( pTrigger->name(), mainThreshold );
				lowerEdge=triggerTable.getSuggestedLowerEdge( pTrigger->name(), mainThreshold );
				upperEdge=triggerTable.getSuggestedUpperEdge( pTrigger->name(), mainThreshold );
			}
			catch( std::exception& error) { /* Do nothing. If no binning suggestions have been set for this trigger use the defaults I set above. */ }

			std::unique_ptr<TH1> pHistogram( new TH1F( (triggerNameForPlot+"_v_allThresholdsScaled").c_str(), "This title gets changed by TriggerRatePlot anyway", numberOfBins, lowerEdge, upperEdge ) );
			pHistogram->SetDirectory( pDirectory );
			// Passing thresholdNames tells the TriggerRatePlot to scale all parameters named in that
			// vector along with mainThreshold.
			triggerPlots_.push_back( std::move(l1menu::TriggerRatePlot(*pTrigger,std::move(pHistogram),mainThreshold,thresholdNames)) );

		}

		// When a threshold is tested, I want all the other thresholds to be zero. I'll run through
		// and zero all of them now.
		for( std::vector<std::string>::const_iterator iName=thresholdNames.begin(); iName!=thresholdNames.end(); ++iName )
		{
			pTrigger->parameter(*iName)=0;
		}


		// I want a plot for each of the thresholds, so I'll loop over the threshold names
		for( std::vector<std::string>::const_iterator iThresholdName=thresholdNames.begin(); iThresholdName!=thresholdNames.end(); ++iThresholdName )
		{
			unsigned int numberOfBins=100;
			float lowerEdge=0;
			float upperEdge=100;
			try
			{
				numberOfBins=triggerTable.getSuggestedNumberOfBins( pTrigger->name(), *iThresholdName );
				lowerEdge=triggerTable.getSuggestedLowerEdge( pTrigger->name(), *iThresholdName );
				upperEdge=triggerTable.getSuggestedUpperEdge( pTrigger->name(), *iThresholdName );
			}
			catch( std::exception& error) { /* Do nothing. If no binning suggestions have been set for this trigger use the defaults I set above. */ }

			std::unique_ptr<TH1> pHistogram( new TH1F( (triggerNameForPlot+"_v_"+(*iThresholdName)).c_str(), "This title gets changed by TriggerRatePlot anyway", numberOfBins, lowerEdge, upperEdge ) );
			pHistogram->SetDirectory( pDirectory );
			triggerPlots_.push_back( std::move(l1menu::TriggerRatePlot(*pTrigger,std::move(pHistogram),*iThresholdName)) );
		}

	} // end of loop over the triggers in the menu
}

l1menu::MenuRatePlots::MenuRatePlots( const TDirectory* pPreExistingPlotDirectory )
{
	// Before making any histograms make sure errors are done properly
	TH1::SetDefaultSumw2();

	// Loop over all of the histograms in the directory.
	TList* pListOfKeys=pPreExistingPlotDirectory->GetListOfKeys();
	std::string oldKeyName;

	for( int index=0; index<pListOfKeys->GetEntries(); ++index )
	{
		TKey* pKey=dynamic_cast<TKey*>( pListOfKeys->At(index) );
		// Only use the highest cycle number for each key
		if( oldKeyName==pKey->GetName() ) continue;
		oldKeyName=pKey->GetName();

		TH1* pHistogram=dynamic_cast<TH1*>( pKey->ReadObj() );

		if( pHistogram!=NULL )
		{
			try
			{
				l1menu::TriggerRatePlot ratePlotFromHistogram( pHistogram );
				triggerPlots_.push_back( std::move(ratePlotFromHistogram) );
			}
			catch( std::exception& error )
			{
				std::cerr << "Couldn't create TriggerRatePlot for " << pHistogram->GetName() << " because: " << error.what() << std::endl;
			}

		} // end of "if( dynamic_cast to TH1* successful )"

	} // end of loop over the keys in the file
}

void l1menu::MenuRatePlots::addEvent( const l1menu::IEvent& event )
{
	// Loop over each of the TriggerRatePlots and add the event to each of them.
	for( auto& ratePlot : triggerPlots_ )
	{
		ratePlot.addEvent( event );
	}
}

void l1menu::MenuRatePlots::addSample( const l1menu::ISample& sample )
{
	// Rather than looping over the TriggerRatePlots and calling addSample on each one,
	// I'll use this static TriggerRatePlot method which does the same job but can be
	// much faster.
	l1menu::TriggerRatePlot::addSample( sample, triggerPlots_ );
}

void l1menu::MenuRatePlots::setDirectory( TDirectory* pDirectory )
{
	// Loop over each of the TriggerRatePlots and individually set the directory.
	for( auto& ratePlot : triggerPlots_ )
	{
		ratePlot.getPlot()->SetDirectory( pDirectory );
	}
}

const std::vector<l1menu::TriggerRatePlot>& l1menu::MenuRatePlots::triggerRatePlots() const
{
	return triggerPlots_;
}

std::vector<l1menu::TriggerRatePlot>& l1menu::MenuRatePlots::triggerRatePlots()
{
	return triggerPlots_;
}

void l1menu::MenuRatePlots::relinquishOwnershipOfPlots()
{
	// Loop over each of the TriggerRatePlots and individually release them.
	for( std::vector<l1menu::TriggerRatePlot>::iterator iRatePlot=triggerPlots_.begin(); iRatePlot!=triggerPlots_.end(); ++iRatePlot )
	{
		iRatePlot->relinquishOwnershipOfPlot();
	}
}
