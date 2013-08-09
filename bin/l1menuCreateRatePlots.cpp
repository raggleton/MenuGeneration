#include <stdexcept>
#include <iostream>

#include <TSystem.h>
#include <FWCore/FWLite/interface/AutoLibraryLoader.h>
#include <TFile.h>
#include "l1menu/ISample.h"
#include "l1menu/MenuRatePlots.h"
#include "l1menu/IMenuRate.h"
#include "l1menu/TriggerMenu.h"
#include "l1menu/tools/tools.h"

int main( int argc, char* argv[] )
{
	gSystem->Load("libFWCoreFWLite.so");
	AutoLibraryLoader::enable();


	if( argc!=3 && argc!=4 )
	{
		std::string executableName=argv[0];
		size_t lastSlashPosition=executableName.find_last_of('/');
		if( lastSlashPosition!=std::string::npos ) executableName=executableName.substr( lastSlashPosition+1, std::string::npos );
		std::cerr << "   Usage: " << executableName << " <sample filename> <menu filename> [output filename]" << std::endl;
		return -1;
	}

	std::string sampleFilename=argv[1];
	std::string menuFilename=argv[2];
	std::string outputFilename;
	if( argc>3 ) outputFilename=argv[3];
	else outputFilename="rateHistograms.root";

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

		l1menu::TriggerMenu menu;
		std::cout << "Loading menu from file " << menuFilename << std::endl;
		menu.loadMenuFromFile( menuFilename );

		l1menu::MenuRatePlots rateVersusThresholdPlots( menu );

		// Use a smart pointer with a custom deleter that will close the file properly.
		std::unique_ptr<TFile,void(*)(TFile*)> pMyRootFile( new TFile( outputFilename.c_str(), "RECREATE" ), [](TFile*p){p->Write();p->Close();delete p;} );
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
