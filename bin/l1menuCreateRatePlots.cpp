#include <stdexcept>
#include <iostream>

#include <TFile.h>
#include "l1menu/ISample.h"
#include "l1menu/MenuRatePlots.h"
#include "l1menu/IMenuRate.h"
#include "l1menu/TriggerMenu.h"
#include "l1menu/tools/miscellaneous.h"
#include "l1menu/tools/fileIO.h"
#include "l1menu/tools/CommandLineParser.h"

void printUsage( const std::string& executableName, std::ostream& output=std::cout )
{
	output << "Usage:" << "\n"
			<< "\t" << executableName << " [--output <output filename>] [--original-binning] <sample filename> <menu filename>" << "\n"
			<< "\t" << "\t" << "Creates trigger rate plots using the menu and sample provided. The \"output\" option allows" << "\n"
			<< "\t" << "\t" << "you to specify the filename for the output (default is \"rateHistograms.root\"). The" << "\n"
			<< "\t" << "\t" << "\"original-binning\" option will use the binning that was used in the L1Menu2015.C macro." << "\n"
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
	std::string outputFilename="rateHistograms.root"; // default value if not specified on the command line

	l1menu::tools::CommandLineParser commandLineParser;
	try
	{
		commandLineParser.addOption( "help", l1menu::tools::CommandLineParser::NoArgument );
		commandLineParser.addOption( "output", l1menu::tools::CommandLineParser::RequiredArgument );
		commandLineParser.addOption( "original-binning", l1menu::tools::CommandLineParser::NoArgument );
		commandLineParser.parse( argc, argv );

		if( commandLineParser.optionHasBeenSet( "help" ) )
		{
			printUsage( commandLineParser.executableName() );
			return 0;
		}

		if( commandLineParser.optionHasBeenSet( "output" ) ) outputFilename=commandLineParser.optionArguments("output").back();
		if( commandLineParser.optionHasBeenSet( "original-binning" ) ) l1menu::tools::setBinningToL1Menu2015Values();
		if( commandLineParser.nonOptionArguments().size()<2 ) throw std::runtime_error( "Not enough command line arguments" );

		const std::vector<std::string>& arguments=commandLineParser.nonOptionArguments();
		sampleFilename=arguments[0];
		menuFilename=arguments[1];
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

		std::cout << "Loading menu from file " << menuFilename << std::endl;
		std::unique_ptr<l1menu::TriggerMenu> pMenu=l1menu::tools::loadMenu( menuFilename );

		l1menu::MenuRatePlots rateVersusThresholdPlots( *pMenu );

		// Use a smart pointer with a custom deleter that will close the file properly.
		std::unique_ptr<TFile,std::function<void(TFile*)>> pMyRootFile( new TFile( outputFilename.c_str(), "RECREATE" ),
				[outputFilename](TFile*p) // Use a lambda function to automatically write the file.
				{
					p->Write();p->Close();delete p;
					std::cout << "Rate plots written to file \"" << outputFilename << "\"" << std::endl;
				} );

		rateVersusThresholdPlots.setDirectory( pMyRootFile.get() );
		rateVersusThresholdPlots.relinquishOwnershipOfPlots();

		std::cout << "Calculating rate plots..." << std::endl;
		rateVersusThresholdPlots.addSample( *pSample );
	}
	catch( std::exception& error )
	{
		std::cerr << "Exception caught: " << error.what() << std::endl;
		return -1;
	}

	return 0;
}
