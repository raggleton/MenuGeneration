#include "l1menu/scalings/MuonScaling.h"

#include <stdexcept>
#include "l1menu/TriggerRatePlot.h"
#include "l1menu/MenuRatePlots.h"
#include "l1menu/IMenuRate.h"

namespace l1menu
{
	namespace scalings
	{
		/** @brief Private members for the MuonScaling class, using the pimple idiom.
		 *
		 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
		 * @date 16/Oct/2013
		 */
		class MuonScalingPrivateMembers
		{
		public:
			std::string detailedDescription_;
		};

	} // end of namespace scalings
} // end of namespace l1menu

l1menu::scalings::MuonScaling::MuonScaling( const std::string& muonScalingFilename )
	: pImple( new MuonScalingPrivateMembers )
{
	pImple->detailedDescription_="Muon scaling filename: '"+muonScalingFilename+"'";
}

l1menu::scalings::MuonScaling::MuonScaling( const std::string& muonScalingFilename, const std::string& unscaledRatesFilename )
	: pImple( new MuonScalingPrivateMembers )
{
	pImple->detailedDescription_="Muon scaling filename: '"+muonScalingFilename+"', unscaled rate filename: '"+unscaledRatesFilename+"'";
}

l1menu::scalings::MuonScaling::~MuonScaling()
{
	// No operation
}

std::string l1menu::scalings::MuonScaling::briefDescription()
{
	return "Muon scaling";
}

std::string l1menu::scalings::MuonScaling::detailedDescription()
{
	return pImple->detailedDescription_;
}

std::unique_ptr<l1menu::TriggerRatePlot> l1menu::scalings::MuonScaling::scale( const l1menu::TriggerRatePlot& unscaledPlot )
{
	std::unique_ptr<l1menu::TriggerRatePlot> pReturnValue;
	return pReturnValue;
}

std::unique_ptr<l1menu::MenuRatePlots> l1menu::scalings::MuonScaling::scale( const l1menu::MenuRatePlots& unscaledPlots )
{
	std::unique_ptr<l1menu::MenuRatePlots> pReturnValue;
	return pReturnValue;
}

std::unique_ptr<l1menu::IMenuRate> l1menu::scalings::MuonScaling::scale( const l1menu::IMenuRate& unscaledMenuRate )
{
	std::unique_ptr<l1menu::IMenuRate> pReturnValue;
	return pReturnValue;
}
