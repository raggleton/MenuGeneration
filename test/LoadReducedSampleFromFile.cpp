#include <stdexcept>
#include <iostream>

#include <TSystem.h>
#include <FWCore/FWLite/interface/AutoLibraryLoader.h>
#include <TFile.h>
#include "l1menu/ReducedSample.h"
#include "l1menu/MenuRatePlots.h"
#include "l1menu/IMenuRate.h"
#include "l1menu/ITriggerRate.h"
#include "l1menu/TriggerMenu.h"
#include "l1menu/ITrigger.h"
#include "l1menu/tools/miscellaneous.h"
#include "l1menu/tools/fileIO.h"
#include <cmath>

int main( int argc, char* argv[] )
{
	gSystem->Load("libFWCoreFWLite.so");
	AutoLibraryLoader::enable();

	if( !(argc==2 || argc==3) )
	{
		std::string executableName=argv[0];
		size_t lastSlashPosition=executableName.find_last_of('/');
		if( lastSlashPosition!=std::string::npos ) executableName=executableName.substr( lastSlashPosition+1, std::string::npos );
		std::cerr << "   Usage: " << executableName << " <reduced sample filename> [menu filename]" << std::endl;
		return -1;
	}

	std::string sampleFilename=argv[1];
	std::string menuFilename;
	if( argc>2 ) menuFilename=argv[2];

	try
	{
		const float scaleToKiloHz=1.0/1000.0;
		const float orbitsPerSecond=11246;
		const float bunchSpacing=25;
		float numberOfBunches;
		if( bunchSpacing==50 ) numberOfBunches=1380;
		else if( bunchSpacing==25 ) numberOfBunches=2760;
		else throw std::logic_error( "The number of bunches has not been programmed for the bunch spacing selected" );


		std::cout << "Loading ReducedSample from the file " << sampleFilename << std::endl;
		l1menu::ReducedSample mySample( sampleFilename );
		mySample.setEventRate( orbitsPerSecond*numberOfBunches*scaleToKiloHz );

		std::unique_ptr<l1menu::TriggerMenu> pMenu;
		if( menuFilename.empty() )
		{
			std::cout << "No menu file was specified, so using the full menu used to create the ReducedSample" << std::endl;
			pMenu.reset( new l1menu::TriggerMenu( mySample.getTriggerMenu() ) );
		}
		else
		{
			std::cout << "Loading menu from file " << menuFilename << std::endl;
			pMenu=l1menu::tools::loadMenu( menuFilename );
		}

//		for( size_t triggerNumber=0; triggerNumber<menu.numberOfTriggers(); ++triggerNumber )
//		{
//			const l1menu::ITrigger& trigger=menu.getTrigger( triggerNumber );
//
//			std::cout << "After loading trigger " << trigger.name();
//			for( const auto& name : trigger.parameterNames() )
//			{
//				std::cout << " (" << name << "=" << trigger.parameter(name) << ")";
//			}
//			std::cout << "\n";
//		}

		l1menu::MenuRatePlots rateVersusThresholdPlots( *pMenu );

		std::cout << "Calculating rate plots..." << std::endl;
		rateVersusThresholdPlots.addSample( mySample );

		// Use a smart pointer with a custom deleter that will close the file properly.
		std::unique_ptr<TFile,void(*)(TFile*)> pMyRootFile( new TFile( "reducedRateHistograms.root", "RECREATE" ), [](TFile*p){p->Write();p->Close();delete p;} );
		rateVersusThresholdPlots.setDirectory( pMyRootFile.get() );
		rateVersusThresholdPlots.relinquishOwnershipOfPlots();

		std::cout << "Calculating fractions..." << std::endl;

		std::shared_ptr<const l1menu::IMenuRate> pRates=mySample.rate(*pMenu);

		l1menu::tools::dumpTriggerRates( std::cout, *pRates );
	}
	catch( std::exception& error )
	{
		std::cerr << "Exception caught: " << error.what() << std::endl;
		return -1;
	}

	return 0;
}
