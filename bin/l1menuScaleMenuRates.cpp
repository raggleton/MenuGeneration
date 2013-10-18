#include <stdexcept>
#include <iostream>
#include <fstream>

#include "l1menu/tools/CommandLineParser.h"
#include "l1menu/tools/fileIO.h"
#include "l1menu/IMenuRate.h"
#include "l1menu/TriggerMenu.h"
#include "l1menu/MenuRatePlots.h"
#include "l1menu/scalings/MCDataScaling.h"
#include "l1menu/scalings/MuonScaling.h"
#include <TFile.h>

namespace // Unnamed namespace for things only used in this file
{

} // end of the unnamed namespace

void printUsage( const std::string& executableName, std::ostream& output=std::cout )
{
	output << "Usage:" << "\n"
			<< "\t" << executableName << " <menu rate to scale 1> [menu rate to scale 2] [menu rate to scale 3]..." << "\n"
			<< "\t" << "\t" << "Currently just a testing ground while I play with xerces for the MenuRate file format." << "\n"
			<< "\t" << "\t" << "In future this program will load a MenuRate from an xml file and perform scaling on it." << "\n"
			<< "\n"
			<< "\t" << executableName << " --help" << "\n"
			<< "\t" << "\t" << "prints this help message" << "\n"
			<< std::endl;
}

int main( int argc, char* argv[] )
{
	std::cerr << "*** This program currently doesn't work. Only use for development. ***" << std::endl;

	std::vector<std::string> menuRates;
	std::string muonScalingFilename; // The filename of the file used to scale muon rates. Optional and can be empty.
	std::string offlineScalingFilename; // The filename of the file used to scale thresholds from online to offline.
	// List of all the scalings that should be applied
	std::vector< std::unique_ptr<l1menu::IScaling> > scalingsToApply;
	std::string unscaledRatesFilename;


	l1menu::tools::CommandLineParser commandLineParser;
	try
	{
		commandLineParser.addOption( "help", l1menu::tools::CommandLineParser::NoArgument );
		commandLineParser.addOption( "muonscaling", l1menu::tools::CommandLineParser::RequiredArgument );
		commandLineParser.addOption( "rateplots", l1menu::tools::CommandLineParser::RequiredArgument );
		commandLineParser.addOption( "offlinescaling", l1menu::tools::CommandLineParser::RequiredArgument );
		commandLineParser.addOption( "montecarloscaling", l1menu::tools::CommandLineParser::RequiredArgument );
		commandLineParser.addOption( "datascaling", l1menu::tools::CommandLineParser::RequiredArgument );
		commandLineParser.parse( argc, argv );

		if( commandLineParser.optionHasBeenSet( "help" ) )
		{
			printUsage( commandLineParser.executableName() );
			return 0;
		}

		if( commandLineParser.nonOptionArguments().empty() ) throw std::runtime_error( "You need to specify a filename for at least one MenuRate" );
		if( commandLineParser.optionHasBeenSet("rateplots") ) unscaledRatesFilename=commandLineParser.optionArguments("rateplots").back();
		if( commandLineParser.optionHasBeenSet("muonscaling") )
		{
			if( unscaledRatesFilename.empty() ) throw std::runtime_error( "Scaling for muons also requires the unscaled rate plots set with the 'rateplots' option" );
			muonScalingFilename=commandLineParser.optionArguments("muonscaling").back();
			scalingsToApply.push_back( std::unique_ptr<l1menu::IScaling>( new l1menu::scalings::MuonScaling(muonScalingFilename,unscaledRatesFilename) ) );
		}
		if( commandLineParser.optionHasBeenSet("offlinescaling") ) offlineScalingFilename=commandLineParser.optionArguments("offlinescaling").back();
		if( commandLineParser.optionHasBeenSet("montecarloscaling") || commandLineParser.optionHasBeenSet("datascaling") )
		{
			if( !commandLineParser.optionHasBeenSet("montecarloscaling") ) throw std::runtime_error( "If the 'datascaling' option is set then 'montecarloscaling' must also be set.");
			else if( !commandLineParser.optionHasBeenSet("datascaling") ) throw std::runtime_error( "If the 'montecarloscaling' option is set then 'datascaling' must also be set.");
			else
			{
				if( unscaledRatesFilename.empty() ) throw std::runtime_error( "Scaling for data/Monte Carlo also requires the unscaled rate plots set with the 'rateplots' option" );
				std::string monteCarloFilename=commandLineParser.optionArguments("montecarloscaling").back();
				std::string dataFilename=commandLineParser.optionArguments("datascaling").back();
				scalingsToApply.push_back( std::unique_ptr<l1menu::IScaling>( new l1menu::scalings::MCDataScaling(monteCarloFilename,dataFilename,unscaledRatesFilename) ) );
			}
		}

		if( scalingsToApply.empty() ) throw std::runtime_error( "No scalings have been requested on the command line. Nothing will be done.");
		menuRates=commandLineParser.nonOptionArguments();
	} // end of try block
	catch( std::exception& error )
	{
		std::cerr << "Error parsing the command line: " << error.what() << std::endl;
		printUsage( commandLineParser.executableName(), std::cerr );
		return -1;
	}


	try
	{
		//
		// First scale all of the rate plots
		//
		if( !unscaledRatesFilename.empty() )
		{
			std::cout << "Scaling " << unscaledRatesFilename << " with..." << std::endl;
			std::unique_ptr<TFile> pRatePlotsRootFile( TFile::Open( unscaledRatesFilename.c_str() ) );
			std::unique_ptr<l1menu::MenuRatePlots> pRatePlots( new l1menu::MenuRatePlots( pRatePlotsRootFile.get() ) );

			for( const auto& pScaling : scalingsToApply )
			{
				std::cout << "   " << pScaling->briefDescription() << std::endl;
				// Replace the current menu with the newly scaled one, and repeat until all scaling has been done.
				pRatePlots=pScaling->scale( *pRatePlots );
			}

			std::unique_ptr<TFile> pOutputScaledRatePlotsFile( TFile::Open( "scaledRatePlots.root", "RECREATE" ) );
			pRatePlots->setDirectory( pOutputScaledRatePlotsFile.get() );
			pRatePlots->relinquishOwnershipOfPlots();
			pOutputScaledRatePlotsFile->Write();
			std::cout << "Scaled rate plots written to file " << "scaledRatePlots.root" << std::endl;
		}


		for( const auto& menuRateFilename : menuRates )
		{
			try
			{
				std::cout << "Loading menu rate from file " << menuRateFilename << std::endl;
				std::unique_ptr<l1menu::IMenuRate> pMenuRate=l1menu::tools::loadRate( menuRateFilename );

				std::cout << "Scaling " << menuRateFilename << " with..." << std::endl;
				for( const auto& pScaling : scalingsToApply )
				{
					std::cout << "   " << pScaling->briefDescription() << std::endl;
					// Replace the current menu with the newly scaled one, and repeat until all scaling has been done.
					pMenuRate=pScaling->scale( *pMenuRate );
				}


				//std::unique_ptr<l1menu::IMenuRate> pMenuRate=l1menu::IMenuRate::load( menuRateFilename );
				//menuRateFile.outputToStream( std::cout );
				l1menu::tools::dumpTriggerRates( std::cout, *pMenuRate );

				std::cout << std::endl;
			}
			catch( std::exception& error )
			{
				std::cerr << "Couldn't process file \"" << menuRateFilename << "\" because: " << error.what() << std::endl;
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
