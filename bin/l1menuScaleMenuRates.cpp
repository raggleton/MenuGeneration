#include <stdexcept>
#include <iostream>

#include "l1menu/tools/CommandLineParser.h"
#include "l1menu/IMenuRate.h"
#include "l1menu/TriggerMenu.h"

namespace // Unnamed namespace for things only used in this file
{

} // end of the unnamed namespace

void printUsage( const std::string& executableName, std::ostream& output=std::cout )
{
	output << "Usage:" << "\n"
			<< "\t" << executableName << " <menu to scale 1> [menu to scale 2] [menu to scale 3]..." << "\n"
			<< "\t" << "\t" << "Currently just a testing ground while I play with xerces for the MenuRate file format." << "\n"
			<< "\t" << "\t" << "In future this program will load a MenuRate from an xml file and perform scaling on it." << "\n"
			<< "\n"
			<< "\t" << executableName << " --help" << "\n"
			<< "\t" << "\t" << "prints this help message"
			<< "\n"
			<< std::endl;
}

int main( int argc, char* argv[] )
{
	std::cerr << "*** This program currently doesn't work. Only use for development. ***" << std::endl;

	std::vector<std::string> menuRates;

	l1menu::tools::CommandLineParser commandLineParser;
	try
	{
		commandLineParser.addOption( "help", l1menu::tools::CommandLineParser::NoArgument );
		commandLineParser.parse( argc, argv );

		if( commandLineParser.optionHasBeenSet( "help" ) )
		{
			printUsage( commandLineParser.executableName() );
			return 0;
		}

		if( commandLineParser.nonOptionArguments().empty() ) throw std::runtime_error( "You need to specify a filename for at least one MenuRate" );

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
				l1menu::TriggerMenu menu;
				std::cout << "Loading menu from file " << menuRateFilename << std::endl;
				menu.loadMenuFromFile( "TestTriggerMenuSave.xml" );
				menu.saveMenuToFile( "TestTriggerMenuSave2.xml" );



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
