#include <stdexcept>
#include <iostream>

#include <TSystem.h>
#include <FWCore/FWLite/interface/AutoLibraryLoader.h>
#include <TFile.h>
#include "l1menu/ISample.h"
#include "l1menu/IMenuRate.h"
#include "l1menu/MenuFitter.h"
#include "l1menu/TriggerTable.h"
#include "l1menu/ITrigger.h"
#include "l1menu/MenuRatePlots.h"
#include "l1menu/tools/tools.h"
#include "l1menu/tools/stringManipulation.h"
#include "l1menu/tools/CommandLineParser.h"

void printUsage( const std::string& executableName, std::ostream& output=std::cout )
{
	output << "Usage:" << "\n"
			<< "\t" << executableName << " [--rateplots <rateplot filename>] <sample filename> <menu filename> <totalRate1> [totalRate2 [totalRate3 [...] ] ]" << "\n"
			<< "\t" << "\t" << "Tries to fit the supplied menu using the sample provided. The optional \"rateplots\" option" << "\n"
			<< "\t" << "\t" << "allows you to reuse a valid file created by l1menuCreateRatePlots which will significantly" << "\n"
			<< "\t" << "\t" << "speed up execution." << "\n"
			<< "\n"
			<< "\t" << executableName << " --help" << "\n"
			<< "\t" << "\t" << "prints this help message"
			<< "\n"
			<< std::endl;
}

int main( int argc, char* argv[] )
{
//	gSystem->Load("libFWCoreFWLite.so");
//	AutoLibraryLoader::enable();

	std::string sampleFilename;
	std::string menuFilename;
	std::string ratePlotsFilename; // Filename for rateplots. This is optional and can be empty.
	std::vector<float> totalRates;

	l1menu::tools::CommandLineParser commandLineParser;
	try
	{
		commandLineParser.addOption( "help", l1menu::tools::CommandLineParser::NoArgument );
		commandLineParser.addOption( "rateplots", l1menu::tools::CommandLineParser::RequiredArgument );
		commandLineParser.parse( argc, argv );

		if( commandLineParser.optionHasBeenSet( "help" ) )
		{
			printUsage( commandLineParser.executableName() );
			return 0;
		}

		if( commandLineParser.optionHasBeenSet( "rateplots" ) ) ratePlotsFilename=commandLineParser.optionArguments("rateplots").back();
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
			std::cout << "Fitting menu for a rate of " << totalRate << "..." << std::endl;
			std::unique_ptr<const l1menu::IMenuRate> pRates=pMenuFitter->fit( totalRate, totalRate*0.05 );
			l1menu::tools::dumpTriggerRates( std::cout, pRates );
		}

	}
	catch( std::exception& error )
	{
		std::cerr << "Exception caught: " << error.what() << std::endl;
		return -1;
	}

	return 0;
}
