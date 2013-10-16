#include <stdexcept>
#include <iostream>
#include <fstream>

#include <TFile.h>
#include "l1menu/ISample.h"
#include "l1menu/IMenuRate.h"
#include "l1menu/TriggerMenu.h"
#include "l1menu/tools/CommandLineParser.h"
#include "l1menu/tools/fileIO.h"

void printUsage( const std::string& executableName, std::ostream& output=std::cout )
{
	output << "Usage:" << "\n"
			<< "\t" << executableName << " [--output <output filename>] [--format < CSV | OLD | XML >] <sample filename> <menu filename>" << "\n"
			<< "\n"
			<< "\t" << executableName << " --help" << "\n"
			<< "\t" << "\t" << "prints this help message" << "\n"
			<< std::endl;
}

int main( int argc, char* argv[] )
{
	std::string sampleFilename;
	std::string menuFilename;
	std::string outputFilename;
	l1menu::tools::FileFormat fileFormat=l1menu::tools::FileFormat::XMLFORMAT;

	l1menu::tools::CommandLineParser commandLineParser;
	try
	{
		commandLineParser.addOption( "output", l1menu::tools::CommandLineParser::RequiredArgument );
		commandLineParser.addOption( "format", l1menu::tools::CommandLineParser::RequiredArgument );
		commandLineParser.addOption( "help", l1menu::tools::CommandLineParser::NoArgument );
		commandLineParser.parse( argc, argv );

		if( commandLineParser.optionHasBeenSet( "help" ) )
		{
			printUsage( commandLineParser.executableName() );
			return 0;
		}

		if( commandLineParser.nonOptionArguments().size()!=2 ) throw std::runtime_error( "Incorrect number of arguments" );
		if( commandLineParser.optionHasBeenSet( "output" ) ) outputFilename=commandLineParser.optionArguments("output").back();
		if( commandLineParser.optionHasBeenSet( "format" ) )
		{
			std::string formatString=commandLineParser.optionArguments("format").back();
			if( formatString=="XML" ) fileFormat=l1menu::tools::FileFormat::XMLFORMAT;
			else if( formatString=="OLD" ) fileFormat=l1menu::tools::FileFormat::OLDFORMAT;
			else if( formatString=="CSV" ) fileFormat=l1menu::tools::FileFormat::CSVFORMAT;
			else throw std::runtime_error( "format must be one of 'XML', 'OLD', or 'CSV'" );
		}

		sampleFilename=commandLineParser.nonOptionArguments()[0];
		menuFilename=commandLineParser.nonOptionArguments()[1];
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

		std::cout << "Calculating rates..." << std::endl;

		std::shared_ptr<const l1menu::IMenuRate> pRates=pSample->rate(*pMenu);

		if( !outputFilename.empty() )
		{
			std::ofstream outputFile( outputFilename );
			if( !outputFile.is_open() ) std::cerr << "ERROR unable to open " << outputFilename << " to store the output" << std::endl;
			else
			{
				l1menu::tools::dumpTriggerRates( outputFile, *pRates, fileFormat );
				std::cout << "Output saved to " << outputFilename << std::endl;
			}
		}
		// Otherwise dump the information to standard output
		else
		{
			std::cout << "outputprefix not specified so dumping results to standard output" << "\n";
			l1menu::tools::dumpTriggerRates( std::cout, *pRates, fileFormat );
		}

	}
	catch( std::exception& error )
	{
		std::cerr << "Exception caught: " << error.what() << std::endl;
		return -1;
	}

	return 0;
}
