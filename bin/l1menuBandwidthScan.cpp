#include <stdexcept>
#include <iostream>

#include "l1menu/ISample.h"
#include "l1menu/IMenuRate.h"
#include "l1menu/BandwidthScan.h"
#include "l1menu/tools/miscellaneous.h"
#include "l1menu/tools/fileIO.h"
#include "l1menu/tools/stringManipulation.h"
#include "l1menu/tools/CommandLineParser.h"

void printUsage( const std::string& executableName, std::ostream& output=std::cout )
{
	output << "Usage:" << "\n"
			<< "\t" << executableName << " <sample filename> <menu filename> <bandwidthFrom> <bandwidthTo>" << "\n"
			<< "\t" << "\t" << "Increases all thresholds steadily while keeping the bandwidth share of each trigger" << "\n"
			<< "\t" << "\t" << "moderately the same. In this way it plots curves of the total bandwidth required" << "\n"
			<< "\t" << "\t" << "versus a set of thresholds. Bandwidths should be given in kHz." << "\n"
			<< "\n"
			<< "\t" << executableName << " --help" << "\n"
			<< "\t" << "\t" << "prints this help message"
			<< "\n"
			<< std::endl;
}

int main( int argc, char* argv[] )
{
	std::cerr << "*** This program currently doesn't work. Only use for development. ***" << std::endl;

	std::string sampleFilename;
	std::string menuFilename;
	float bandwidthFrom;
	float bandwidthTo;

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

		if( commandLineParser.nonOptionArguments().size()!=4 ) throw std::runtime_error( "Incorrect number of command line arguments" );

		const std::vector<std::string>& arguments=commandLineParser.nonOptionArguments();
		sampleFilename=arguments[0];
		menuFilename=arguments[1];
		bandwidthFrom=l1menu::tools::convertStringToFloat( arguments[2] );
		bandwidthTo=l1menu::tools::convertStringToFloat( arguments[3] );
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

		l1menu::BandwidthScan bandwidthScan( *pSample );
		std::cout << "Loading menu from file " << menuFilename << std::endl;
		bandwidthScan.loadMenuFromFile( menuFilename );

		std::vector<const l1menu::IMenuRate*> results=bandwidthScan.scan( bandwidthFrom, bandwidthTo );

		std::cout << "Results:" << std::endl;
		for( const auto& pMenuRate : results )
		{
			l1menu::tools::dumpTriggerRates( std::cout, *pMenuRate );
		}

	}
	catch( std::exception& error )
	{
		std::cerr << "Exception caught: " << error.what() << std::endl;
		return -1;
	}

	return 0;
}
