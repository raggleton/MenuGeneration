#include "l1menu/FullSample.h"
#include "l1menu/TriggerMenu.h"
#include "l1menu/ReducedSample.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <stdexcept>


int main( int argc, char* argv[] )
{
	std::string outputFilename="reducedSample.proto";

	if( argc<3 )
	{
		std::string executableName=argv[0];
		size_t lastSlashPosition=executableName.find_last_of('/');
		if( lastSlashPosition!=std::string::npos ) executableName=executableName.substr( lastSlashPosition+1, std::string::npos );
		std::cerr << "   Usage: " << executableName << " <menu file> <input ntuple 1> [input ntuple 2 [...] ]" << "\n"
				<< " Creates an l1menu::ReducedSample in protobuf format from the input files specified on the "
				<< "command line. The output file is called \"" << outputFilename << "\"." << std::endl;
		return -1;
	}

	std::string menuFilename=argv[1];

	std::vector<std::string> inputFilenames;
	for( int index=2; index<argc; ++index ) inputFilenames.push_back( argv[index] );


	try
	{
		std::cout << "Loading menu from file " << menuFilename << std::endl;
		std::unique_ptr<l1menu::TriggerMenu> pMyMenu=l1menu::tools::loadMenu( menuFilename );

		l1menu::ReducedSample outputReducedSample( *pMyMenu );

		for( const auto& filename : inputFilenames )
		{
			l1menu::FullSample inputSample;
			inputSample.loadFile(filename);
			outputReducedSample.addSample( inputSample );
		}

		outputReducedSample.saveToFile( outputFilename );
		std::cout << "Reduced sample saved to " << outputFilename << std::endl;
	}
	catch( std::exception& error )
	{
		std::cerr << "Exception caught: " << error.what() << std::endl;
	}

	return 0;
}
