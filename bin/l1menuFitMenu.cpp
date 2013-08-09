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
#include "l1menu/tools/tools.h"
#include "l1menu/tools/stringManipulation.h"

int main( int argc, char* argv[] )
{
	gSystem->Load("libFWCoreFWLite.so");
	AutoLibraryLoader::enable();


	if( argc<4 )
	{
		std::string executableName=argv[0];
		size_t lastSlashPosition=executableName.find_last_of('/');
		if( lastSlashPosition!=std::string::npos ) executableName=executableName.substr( lastSlashPosition+1, std::string::npos );
		std::cerr << "   Usage: " << executableName << " <sample filename> <menu filename> <totalRate1> [totalRate2 [totalRate3 [...] ] ]" << std::endl;
		return -1;
	}

	std::string sampleFilename=argv[1];
	std::string menuFilename=argv[2];
	std::vector<float> totalRates;
	for( int argument=3; argument<argc; ++argument )
	{
		try
		{
			totalRates.push_back( l1menu::tools::convertStringToFloat(argv[argument]) );
		}
		catch( std::runtime_error& exception )
		{
			std::cerr << "Problem converting argument " << argument << " into a float: " << exception.what() << std::endl;
			return -1;
		}
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

		l1menu::MenuFitter menuFitter( *pSample );
		std::cout << "Loading menu from file " << menuFilename << std::endl;
		menuFitter.loadMenuFromFile( menuFilename );

		for( const auto& totalRate : totalRates )
		{
			std::cout << "Fitting menu for a rate of " << totalRate << "..." << std::endl;
			std::unique_ptr<const l1menu::IMenuRate> pRates=menuFitter.fit( totalRate, totalRate*0.05 );
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
