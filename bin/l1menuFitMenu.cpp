#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>

#include "l1menu/ISample.h"
#include "l1menu/IMenuRate.h"
#include "l1menu/MenuFitter.h"
#include "l1menu/TriggerTable.h"
#include "l1menu/ITrigger.h"
#include "l1menu/TriggerMenu.h"
#include "l1menu/MenuRatePlots.h"
#include "l1menu/MenuRateMuonScaling.h"
#include "l1menu/MenuRateOfflineScaling.h"
#include "l1menu/tools/tools.h"
#include "l1menu/tools/stringManipulation.h"
#include "l1menu/tools/CommandLineParser.h"
#include "l1menu/scalings/MuonAndMCScaling.h"
#include "l1menu/scalings/OnlineToOfflineScaling.h"
#include <TFile.h>
#include <TH1.h>

void printUsage( const std::string& executableName, std::ostream& output=std::cout )
{
	output << "Usage:" << "\n"
			<< "\t" << executableName << " [--rateplots <rateplot filename>] [--outputprefix <output prefix>] [--muonscaling <muon scaling filename>] [--offlinescaling <offline scaling filename>] <sample filename> <menu filename> <totalRate1> [totalRate2 [totalRate3 [...] ] ]" << "\n"
			<< "\t" << "\t" << "Tries to fit the supplied menu using the sample provided. The optional \"rateplots\" option" << "\n"
			<< "\t" << "\t" << "allows you to reuse a valid file created by l1menuCreateRatePlots which will significantly" << "\n"
			<< "\t" << "\t" << "speed up execution. If the option \"outputprefix\" is supplied the results will be saved to" << "\n"
			<< "\t" << "\t" << "a file with the name <output prefix>_<totalRate>kHz.txt. If this option isn't provided the" << "\n"
			<< "\t" << "\t" << "output will be printed to standard output." << "\n"
			<< "\t" << "\t" << "If \"muonscaling\" is set, the specified file will be used to calculate the muon scaling." << "\n"
			<< "\n"
			<< "\t" << executableName << " --help" << "\n"
			<< "\t" << "\t" << "prints this help message"
			<< "\n"
			<< std::endl;
}

int main( int argc, char* argv[] )
{
	std::string sampleFilename;
	std::string menuFilename;
	std::string ratePlotsFilename; // Filename for rateplots. This is optional and can be empty.
	std::string outputPrefix; // A prefix for output filenames. This is optional and can be empty, in which case no files are produced.
	std::string muonScalingFilename; // The filename of the file used to scale muon rates. Optional and can be empty.
	std::string offlineScalingFilename; // The filename of the file used to scale thresholds from online to offline.
	std::vector<float> totalRates;

	l1menu::tools::CommandLineParser commandLineParser;
	try
	{
		commandLineParser.addOption( "help", l1menu::tools::CommandLineParser::NoArgument );
		commandLineParser.addOption( "rateplots", l1menu::tools::CommandLineParser::RequiredArgument );
		commandLineParser.addOption( "outputprefix", l1menu::tools::CommandLineParser::RequiredArgument );
		commandLineParser.addOption( "muonscaling", l1menu::tools::CommandLineParser::RequiredArgument );
		commandLineParser.addOption( "offlinescaling", l1menu::tools::CommandLineParser::RequiredArgument );
		commandLineParser.parse( argc, argv );

		if( commandLineParser.optionHasBeenSet( "help" ) )
		{
			printUsage( commandLineParser.executableName() );
			return 0;
		}

		if( commandLineParser.optionHasBeenSet( "rateplots" ) ) ratePlotsFilename=commandLineParser.optionArguments("rateplots").back();
		if( commandLineParser.optionHasBeenSet( "outputprefix" ) ) outputPrefix=commandLineParser.optionArguments("outputprefix").back();
		if( commandLineParser.optionHasBeenSet( "muonscaling" ) ) muonScalingFilename=commandLineParser.optionArguments("muonscaling").back();
		if( commandLineParser.optionHasBeenSet( "offlinescaling" ) ) offlineScalingFilename=commandLineParser.optionArguments("offlinescaling").back();
		if( commandLineParser.nonOptionArguments().size()<3 ) throw std::runtime_error( "Not enough command line arguments" );

		const std::vector<std::string>& arguments=commandLineParser.nonOptionArguments();
		sampleFilename=arguments[0];
		menuFilename=arguments[1];
		for( size_t index=2; index<arguments.size(); ++index )
		{
			try
			{
				totalRates.push_back( l1menu::tools::convertStringToFloat(arguments[index]) );
			}
			catch( std::runtime_error& exception )
			{
				std::cerr << "Problem converting argument " << index << " into a float: " << exception.what() << std::endl;
				return -1;
			}
		}
	} // end of try block
	catch( std::exception& error )
	{
		std::cerr << "Error parsing the command line: " << error.what() << std::endl;
		printUsage( commandLineParser.executableName(), std::cerr );
		return -1;
	}


	try
	{
		const float scaleToKiloHz=1.0/1000.0;
		const float orbitsPerSecond=11246;
		const float bunchSpacing=25;
		float numberOfBunches;
		if( bunchSpacing==50 ) numberOfBunches=1380;
		else if( bunchSpacing==25 ) numberOfBunches=2760;
		else throw std::logic_error( "The number of bunches has not been programmed for the bunch spacing selected" );


		std::cout << "Loading sample from the file " << sampleFilename << std::endl;
		std::unique_ptr<l1menu::ISample> pSample=l1menu::tools::loadSample( sampleFilename );
		pSample->setEventRate( orbitsPerSecond*numberOfBunches*scaleToKiloHz );

		std::unique_ptr<l1menu::MenuFitter> pMenuFitter;
		if( ratePlotsFilename.empty() ) pMenuFitter.reset( new l1menu::MenuFitter(*pSample) );
		else
		{
			// If the user has specified a rateplots file on the command line try and
			// load it up and use it to create the MenuFitter. This will save a significant
			// amount of time during the MenuFitter constructor.
			std::unique_ptr<TFile> pRatePlotsRootFile( TFile::Open( ratePlotsFilename.c_str() ) );
			l1menu::MenuRatePlots ratePlots( pRatePlotsRootFile.get() );

			pMenuFitter.reset( new l1menu::MenuFitter( *pSample, ratePlots ) );
		}

		std::cout << "Loading menu from file " << menuFilename << std::endl;
		pMenuFitter->loadMenuFromFile( menuFilename );

		for( const auto& totalRate : totalRates )
		{
			std::cout << "Fitting menu for a rate of " << totalRate << "kHz..."; std::cout.flush();
			try
			{
				l1menu::scalings::MuonAndMCScaling muonAndMCScaling( muonScalingFilename,
						"/home/xtaldaq/CMSSWReleases/CMSSW_5_3_4/src/v19Results/L1RateHist_8TeV45PU_25nsMCMC_FallbackThr1_rates.root",
						"/home/xtaldaq/CMSSWReleases/CMSSW_5_3_4/src/v19Results/L1RateHist_8TeV45PU_25nsDataMC_FallbackThr1_rates.root" );
				l1menu::scalings::OnlineToOfflineScaling onlineToOfflineScaling( offlineScalingFilename );

				// Use a smart pointer with a custom deleter that will close the file properly.
				std::unique_ptr<TFile,void(*)(TFile*)> pOriginalOutputFile( new TFile( "originalHistograms.root", "RECREATE" ), [](TFile*p){p->Write();p->Close();delete p;} );
				std::unique_ptr<TFile,void(*)(TFile*)> pRootOnlineOutputFile( new TFile( "scaledOnlineHistograms.root", "RECREATE" ), [](TFile*p){p->Write();p->Close();delete p;} );
				std::unique_ptr<TFile,void(*)(TFile*)> pRootOfflineOutputFile( new TFile( "scaledOfflineHistograms.root", "RECREATE" ), [](TFile*p){p->Write();p->Close();delete p;} );
				for( size_t triggerNumber=0; triggerNumber<pMenuFitter->menu().numberOfTriggers(); ++triggerNumber )
				{
					const l1menu::TriggerRatePlot& originalPlot=pMenuFitter->triggerRatePlot(triggerNumber);
					std::unique_ptr<l1menu::TriggerRatePlot> pScaledPlot=muonAndMCScaling.scaleTriggerRatePlot( originalPlot );
					std::unique_ptr<l1menu::TriggerRatePlot> pOfflinePlot=onlineToOfflineScaling.scaleTriggerRatePlot( *pScaledPlot );
					const_cast<l1menu::TriggerRatePlot&>(originalPlot).getPlot()->SetDirectory( pOriginalOutputFile.get() );
					pScaledPlot->getPlot()->SetDirectory( pRootOnlineOutputFile.get() );
					pOfflinePlot->getPlot()->SetDirectory( pRootOfflineOutputFile.get() );
					const_cast<l1menu::TriggerRatePlot&>(originalPlot).relinquishOwnershipOfPlot();
					pScaledPlot->relinquishOwnershipOfPlot();
					pOfflinePlot->relinquishOwnershipOfPlot();
				}

				throw std::runtime_error( "Can't be arsed waiting for the fit");

				std::shared_ptr<const l1menu::IMenuRate> pRates=pMenuFitter->fit( totalRate, totalRate*0.05 );

				std::shared_ptr<const l1menu::IMenuRate> pScaledRates;
				if( !muonScalingFilename.empty() )
				{
					pScaledRates.reset( new l1menu::MenuRateMuonScaling( pRates, muonScalingFilename, *pMenuFitter ) );
				}
				else pScaledRates=pRates;

				std::shared_ptr<const l1menu::IMenuRate> pOfflineRates;
				if( !offlineScalingFilename.empty() )
				{
					pOfflineRates.reset( new l1menu::MenuRateOfflineScaling( pScaledRates, offlineScalingFilename ) );
				}

				// If the user has specified filenames to save to, save the result to there
				if( !outputPrefix.empty() )
				{
					std::stringstream outputFilename;
					outputFilename << outputPrefix << "_" << totalRate << "kHz.txt";
					std::ofstream outputFile( outputFilename.str() );
					if( !outputFile.is_open() ) std::cerr << "ERROR unable to open " << outputFilename.str() << " to store the output" << std::endl;
					else
					{
						l1menu::tools::dumpTriggerRates( outputFile, *pScaledRates, pOfflineRates.get() );
						std::cout << "Output saved to " << outputFilename.str() << std::endl;
					}
				}
				// Otherwise dump the information to standard output
				else
				{
					std::cout << "outputprefix not specified so dumping results to standard output" << "\n";
					l1menu::tools::dumpTriggerRates( std::cout, *pScaledRates, pOfflineRates.get() );
				}
			}
			catch( std::exception& error )
			{
				std::cerr << "An exception occured while trying to fit for " << totalRate << "kHz: " << error.what() << "\n";
				std::cout << "--------------------    Start of fit log    --------------------" << "\n"
						<< pMenuFitter->debugLog() << "\n"
						<< "--------------------     End of fit log     --------------------" << "\n";
			}
		}

	}
	catch( std::exception& error )
	{
		std::cerr << "Exception caught: " << error.what() << std::endl;
		return -1;
	}

	return 0;
}
