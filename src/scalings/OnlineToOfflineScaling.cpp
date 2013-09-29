#include "l1menu/scalings/OnlineToOfflineScaling.h"

#include <stdexcept>
#include <iostream>
#include <map>
#include <fstream>
#include "l1menu/TriggerRatePlot.h"
#include "l1menu/MenuRatePlots.h"
#include "l1menu/IMenuRate.h"
#include "l1menu/ITrigger.h"
#include "l1menu/tools/stringManipulation.h"
#include <TH1.h>

//
// Pimple class was implicitly declared, need to explicitly declare and define it
//
namespace l1menu
{
	namespace scalings
	{
		class OnlineToOfflineScalingPrivateMembers
		{
		public:
			// This is a map that uses the trigger name as a key. The value part is a vector
			// of the scalings for each of the thresholds, held as a pair with slope as "first"
			// and the offset as "second".
			std::map<std::string, std::vector<std::pair<float,float> > > triggerScalings_;
			void performScaling( TH1* pHistogramToScale, const std::string& triggerName );
		};

	} // end of namespace scalings
} // end of namespace l1menu

l1menu::scalings::OnlineToOfflineScaling::OnlineToOfflineScaling( const std::string& offlineScalingFilename )
	: pImple_( new OnlineToOfflineScalingPrivateMembers )
{
	// Open the file that contains the scalings
	std::ifstream inputFile( offlineScalingFilename );
	if( !inputFile.is_open() ) throw std::runtime_error( "MenuRateOfflineScaling could not be constructed because the input file "+offlineScalingFilename+" could no be opened" );


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
			pImple_->triggerScalings_[triggerName]=scalings;

		} // end of try block
		catch( std::runtime_error& exception )
		{
			std::cerr << "Some error occured while processing the line \"" << buffer << "\":" << exception.what() << std::endl;
		}
	}
	inputFile.close();

}

l1menu::scalings::OnlineToOfflineScaling::~OnlineToOfflineScaling()
{
	// No operation
}

std::string l1menu::scalings::OnlineToOfflineScaling::briefDescription()
{
	return "Scales online thresholds to offline thresholds.";
}

std::string l1menu::scalings::OnlineToOfflineScaling::detailedDescription()
{
	return "I'll write this later.";
}

std::unique_ptr<l1menu::TriggerRatePlot> l1menu::scalings::OnlineToOfflineScaling::scaleTriggerRatePlot( const l1menu::TriggerRatePlot& unscaledPlot )
{
	// Take a copy and work on that
	std::unique_ptr<l1menu::TriggerRatePlot> pReturnValue( new l1menu::TriggerRatePlot( unscaledPlot ) );

	TH1* pRawPlot=pReturnValue->getPlot();
	// First make sure it's held in memory only. The user can change that later if they want.
	pRawPlot->SetDirectory( nullptr );
	// Delegate to a private method to do the work
	pImple_->performScaling( pRawPlot, pReturnValue->getTrigger().name() );

	return pReturnValue;
}

std::unique_ptr<l1menu::MenuRatePlots> l1menu::scalings::OnlineToOfflineScaling::scaleMenuRatePlots( const l1menu::MenuRatePlots& unscaledPlots )
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

std::shared_ptr<l1menu::IMenuRate> l1menu::scalings::OnlineToOfflineScaling::scaleMenuRate( const l1menu::IMenuRate& unscaledMenuRate )
{
	throw std::logic_error("l1menu::scalings::OnlineToOfflineScaling::scaleMenuRate not implemented yet");
}

void l1menu::scalings::OnlineToOfflineScalingPrivateMembers::performScaling( TH1* pHistogramToScale, const std::string& triggerName )
{
	// If I have no scaling information for this trigger do nothing
	const auto& iFindResult=triggerScalings_.find( triggerName );
	if( iFindResult==triggerScalings_.end() ) return;


	// Currently short of time and just going to scale everything by the first threshold since that's
	// the only plot I want.
	// TODO - fix this
	const std::vector< std::pair<float,float> >& scalings=iFindResult->second;
	float slope=scalings[0].first;
	float offset=scalings[0].second;

	// Take a copy of the original before I change the binning
	std::unique_ptr<TH1> pClone( static_cast<TH1*>(pHistogramToScale->Clone()) );
	pClone->SetDirectory(nullptr);

	// The histogram is scaled by just changing the values along the x-axis because it's
	// a linear scaling. So I change the binning on the x-axis and then copy the contents
	// of the old bins back on top.
	int numberOfBins=pHistogramToScale->GetXaxis()->GetNbins();
	float lowEdge=pHistogramToScale->GetXaxis()->GetXmin();
	float highEdge=pHistogramToScale->GetXaxis()->GetXmax();
	pHistogramToScale->SetBins( numberOfBins, lowEdge*slope+offset, highEdge*slope+offset );

	// Bin contents will have been lost so copy it back over from the clone
	for( int bin=0; bin<=numberOfBins+1; ++bin ) // copy the underflow and overflow too (bin 0 and numberOfBins+1)
	{
		pHistogramToScale->SetBinContent( bin, pClone->GetBinContent(bin) );
		pHistogramToScale->SetBinError( bin, pClone->GetBinError(bin) );
	}
	pHistogramToScale->SetEntries( pClone->GetEntries() );

}
