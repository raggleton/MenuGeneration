#include "l1menu/BandwidthScan.h"

#include <fstream>
#include <string>
#include <stdexcept>
#include <iostream>
#include "l1menu/ISample.h"
#include "l1menu/IMenuRate.h"
#include "l1menu/ITriggerRate.h"
#include "l1menu/ITrigger.h"
#include "l1menu/tools/stringManipulation.h"
#include "l1menu/tools/miscellaneous.h"
#include "l1menu/tools/fileIO.h"
#include "implementation/TriggerMenuWithConstraints.h"


//
// Need to declare the pimple. So far it's just been forward declared.
//
namespace l1menu
{
	/** @brief Private members for the BandwidthScan wrapped up in a compiler firewall.
	 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
	 * @date 26/Sep/2013
	 */
	class BandwidthScanPrivateMembers
	{
	public:
		BandwidthScanPrivateMembers( const l1menu::ISample& newSample )
			: sample(newSample)
		{
		}
		const l1menu::ISample& sample;
		l1menu::implementation::TriggerMenuWithConstraints menu;
	};

}


l1menu::BandwidthScan::BandwidthScan( const l1menu::ISample& sample )
	: pImple_( new BandwidthScanPrivateMembers(sample) )
{
	std::cout << "BandwidthScan has been constructed, but it hasn't been fully implemented yet. It's pretty useless at the moment." << std::endl;
}

l1menu::BandwidthScan::~BandwidthScan()
{
	// No operation
}

std::vector<const l1menu::IMenuRate*> l1menu::BandwidthScan::scan( float fromBandwidth, float toBandwidth )
{
	if( fromBandwidth>toBandwidth ) std::swap( fromBandwidth, toBandwidth );
	std::cout << "Scanning bandwidth from " << fromBandwidth << " to " << toBandwidth << std::endl;

	std::shared_ptr<const l1menu::IMenuRate> pMenuRate=pImple_->sample.rate( pImple_->menu );
	l1menu::tools::dumpTriggerRates( std::cout, *pMenuRate );

	// Run through the bandwidths for each trigger and find out which one is furthest off what it should
	// be and modify that.
	size_t triggerNumber=0;
	for( const auto& pTriggerRate : pMenuRate->triggerRates() )
	{
		float ratio=pTriggerRate->rate()/pMenuRate->totalRate();
		try
		{
			const auto& scalingDetails=pImple_->menu.scalingDetails(triggerNumber);
			std::cout << "Trigger " << pTriggerRate->trigger().name() << " has " << ratio << " of the total bandwidth. Should be " << scalingDetails.bandwidthFraction << std::endl;
		}
		catch( std::exception& error )
		{
			std::cout << "Trigger " << pTriggerRate->trigger().name() << " has " << ratio << " of the total bandwidth. It is not scalable." << std::endl;
		}

		++triggerNumber;
	}


	std::vector<const l1menu::IMenuRate*> returnValue;
	return returnValue;
}

void l1menu::BandwidthScan::loadMenuFromFile( const std::string& filename )
{
	pImple_->menu.loadMenuFromFile( filename );
}

