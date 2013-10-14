#include "TriggerMenuWithConstraints.h"

#include <fstream>
#include <stdexcept>
#include <iostream>
#include "l1menu/ITrigger.h"
#include "l1menu/MenuRatePlots.h"
#include "l1menu/TriggerRatePlot.h"
#include "l1menu/ISample.h"
#include "l1menu/tools/stringManipulation.h"
#include "l1menu/tools/miscellaneous.h"

l1menu::implementation::TriggerMenuWithConstraints::~TriggerMenuWithConstraints()
{
	/* No operation */
}

void l1menu::implementation::TriggerMenuWithConstraints::loadMenuFromFile( const std::string& filename )
{
	std::ifstream file( filename.c_str() );
	if( !file.is_open() ) throw std::runtime_error( "Unable to open file "+filename );

	const size_t bufferSize=200;
	char buffer[bufferSize];

	// The old file format provides the rates it wants for each trigger
	// as an absolute value, whereas I want it as a fraction of the total.
	// So I need to run through the whole file and add up each of these
	// to get a total, then divide each value by the total to get the fraction.
	// Hence I can't do that until I've loaded every trigger. I'll keep track
	// of whether each threshold is locked and the absolute rate requested in
	// this vector.
	std::vector< std::pair<bool,float> > triggerAuxiliaryInfo;
	float totalRequestedRate=0; // This is the sum of all the rates requested for each trigger

	while( file.good() )
	{
		try
		{
			// Get one line at a time
			file.getline( buffer, bufferSize );

			// split the line by whitespace into columns
			std::vector<std::string> tableColumns=l1menu::tools::splitByWhitespace( buffer );

			if( tableColumns.size()==1 && tableColumns[0].empty() ) continue; // Allow blank lines without giving a warning
			if( tableColumns.size()!=12 ) throw std::runtime_error( "The line does not have the correct number of columns" );

			if( addTriggerFromOldFormat( tableColumns ) ) // delegate to the method in the base class
			{
				// If the trigger was created successfully, keep a note of these
				// details so that I can later initiate the auxiliary information.
				// I can't do that until I know the total requestedRate.
				bool lockThresholds=l1menu::tools::convertStringToFloat( tableColumns[11] );
				float requestedRate=l1menu::tools::convertStringToFloat( tableColumns[9] );
				triggerAuxiliaryInfo.push_back( std::make_pair(lockThresholds,requestedRate) );
				totalRequestedRate+=requestedRate;
			}

		} // end of try block
		catch( std::runtime_error& exception )
		{
			std::cerr << "Some error occured while processing the line \"" << buffer << "\":" << exception.what() << std::endl;
		}
	}

	// Now I've processed all the lines I should know the total requested rate and I can
	// initiate the auxiliary information.
	size_t triggerNumber=numberOfTriggers()-triggerAuxiliaryInfo.size(); // There may have been triggers in there previously
	for( const auto auxiliaryInfo : triggerAuxiliaryInfo )
	{
		if( auxiliaryInfo.first ) continue; // If the thresholds are locked don't need any extra information

		const l1menu::ITrigger& trigger=getTrigger( triggerNumber );

		// Add a blank TriggerScalingDetails structure and then start filling it
		scalableTriggers.push_back( TriggerMenuWithConstraints::TriggerScalingDetails() );
		TriggerMenuWithConstraints::TriggerScalingDetails& scalingDetails=scalableTriggers.back();

		scalingDetails.triggerNumber=triggerNumber;
		scalingDetails.bandwidthFraction=auxiliaryInfo.second/totalRequestedRate;
		// leave the scalingDetails.pRatePlot member as a nullptr, if it's required it can be set later

		const std::vector<std::string> thresholdNames=l1menu::tools::getThresholdNames(trigger);
		scalingDetails.mainThreshold=thresholdNames.front();

		const float mainThresholdValue=trigger.parameter(scalingDetails.mainThreshold);
		for( const auto& thresholdName : thresholdNames )
		{
			if( thresholdName==scalingDetails.mainThreshold ) continue;
			scalingDetails.thresholdScalings.push_back( std::make_pair( thresholdName, trigger.parameter(thresholdName)/mainThresholdValue ) );
		}

		++triggerNumber;
	}

	for( const auto& scalingDetails : scalableTriggers )
	{
		std::cout << scalingDetails.triggerNumber << " wants " << scalingDetails.bandwidthFraction << " of the bandwidth." << std::endl;
	}
}

bool l1menu::implementation::TriggerMenuWithConstraints::findAndCopyTriggerRatePlot( const l1menu::MenuRatePlots& menuRatePlots, size_t triggerNumber )
{
	// First find the TriggerScalingDetails structure for this trigger
	TriggerMenuWithConstraints::TriggerScalingDetails* pScalingDetails=nullptr;
	for( auto& scalingDetails : scalableTriggers )
	{
		if( scalingDetails.triggerNumber==triggerNumber ) pScalingDetails=&scalingDetails;
	}

	if( pScalingDetails==nullptr ) throw std::runtime_error( "findAndCopyTriggerRatePlot was asked to find a trigger rate plot for a trigger that is not scalable");
	const l1menu::ITrigger& trigger=getTrigger( triggerNumber );

	for( const auto& triggerRatePlot : menuRatePlots.triggerRatePlots() )
	{
		// See if the plot was made with a trigger equivalent to this one
		if( triggerRatePlot.triggerMatches(trigger) )
		{
			// If it was, take a copy of it
			pScalingDetails->pRatePlot.reset( new l1menu::TriggerRatePlot(triggerRatePlot) );
			return true;
		}
	}

	// If control gets this far then a matching plot wasn't found.
	return false;
}

bool l1menu::implementation::TriggerMenuWithConstraints::createTriggerRatePlot( const l1menu::ISample& sample, size_t triggerNumber )
{
	// First find the TriggerScalingDetails structure for this trigger
	TriggerMenuWithConstraints::TriggerScalingDetails* pScalingDetails=nullptr;
	for( auto& scalingDetails : scalableTriggers )
	{
		if( scalingDetails.triggerNumber==triggerNumber ) pScalingDetails=&scalingDetails;
	}

	if( pScalingDetails==nullptr ) throw std::runtime_error( "createTriggerRatePlot was asked to create a trigger rate plot for a trigger that is not scalable");
	const l1menu::ITrigger& trigger=getTrigger( triggerNumber );

	// Figure out the binning for the plot
	unsigned int numberOfBins=100;
	float lowerEdge=0;
	float upperEdge=100;
	try
	{
		l1menu::TriggerTable& triggerTable=l1menu::TriggerTable::instance();
		numberOfBins=triggerTable.getSuggestedNumberOfBins( trigger.name(), pScalingDetails->mainThreshold );
		lowerEdge=triggerTable.getSuggestedLowerEdge( trigger.name(), pScalingDetails->mainThreshold );
		upperEdge=triggerTable.getSuggestedUpperEdge( trigger.name(), pScalingDetails->mainThreshold );
	}
	catch( std::exception& error) { /* Do nothing. If no binning suggestions have been set for this trigger use the defaults I set above. */ }

	const std::vector<std::string> thresholdNames=l1menu::tools::getThresholdNames(trigger);
	pScalingDetails->pRatePlot.reset( new l1menu::TriggerRatePlot(trigger,trigger.name()+"_v_allThresholdsScaled",numberOfBins,lowerEdge,upperEdge,pScalingDetails->mainThreshold,thresholdNames) );
	pScalingDetails->pRatePlot->addSample( sample );

	return true;
}

const l1menu::implementation::TriggerMenuWithConstraints::TriggerScalingDetails& l1menu::implementation::TriggerMenuWithConstraints::scalingDetails( size_t triggerNumber )
{
	// Loop over the details on record and try and match the trigger number
	TriggerMenuWithConstraints::TriggerScalingDetails* pScalingDetails=nullptr;
	for( auto& scalingDetails : scalableTriggers )
	{
		if( scalingDetails.triggerNumber==triggerNumber ) pScalingDetails=&scalingDetails;
	}

	if( pScalingDetails==nullptr ) throw std::runtime_error( "scaling details were requested for a trigger that is not scalable");

	return *pScalingDetails;
}
