#include <stdexcept>
#include <iostream>
#include <fstream>

#include "l1menu/tools/CommandLineParser.h"
#include "l1menu/tools/fileIO.h"
#include "l1menu/IMenuRate.h"
#include "l1menu/TriggerMenu.h"

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
	std::string montoCarloScalingFilename; // Filename for rate plots from MC that matches the data. If this is set dataScalingFilename must also be set.
	std::string dataScalingFilename;

	l1menu::tools::CommandLineParser commandLineParser;
	try
	{
		commandLineParser.addOption( "help", l1menu::tools::CommandLineParser::NoArgument );
		commandLineParser.addOption( "muonscaling", l1menu::tools::CommandLineParser::RequiredArgument );
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
		if( commandLineParser.optionHasBeenSet("muonscaling") ) muonScalingFilename=commandLineParser.optionArguments("muonscaling").back();
		if( commandLineParser.optionHasBeenSet("offlinescaling") ) offlineScalingFilename=commandLineParser.optionArguments("offlinescaling").back();
		if( commandLineParser.optionHasBeenSet("montecarloscaling") || commandLineParser.optionHasBeenSet("datascaling") )
		{
			if( !commandLineParser.optionHasBeenSet("montecarloscaling") ) throw std::runtime_error( "If the 'datascaling' option is set then 'montecarloscaling' must also be set.");
			else if( !commandLineParser.optionHasBeenSet("datascaling") ) throw std::runtime_error( "If the 'montecarloscaling' option is set then 'datascaling' must also be set.");
			else
			{
				montoCarloScalingFilename=commandLineParser.optionArguments("montecarloscaling").back();
				dataScalingFilename=commandLineParser.optionArguments("datascaling").back();
			}
		}

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

		for( const auto& menuRateFilename : menuRates )
		{
			try
			{
				std::cout << "Loading menu rate from file " << menuRateFilename << std::endl;
				std::unique_ptr<l1menu::IMenuRate> pMenuRate=l1menu::tools::loadRate( menuRateFilename );




				//std::unique_ptr<l1menu::IMenuRate> pMenuRate=l1menu::IMenuRate::load( menuRateFilename );
				//menuRateFile.outputToStream( std::cout );

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
