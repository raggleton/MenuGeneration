#ifndef l1menu_MenuRateMuonScaling_h
#define l1menu_MenuRateMuonScaling_h

#include <vector>
#include <memory>
#include <string>
#include "l1menu/IMenuRate.h"


//
// Forward declarations
//
namespace l1menu
{
	class ITriggerRate;
	class MenuFitter;
}

namespace l1menu
{
	/** @brief Applies scalings for muon pt and isolation to an IMenuRate instance.
	 *
	 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
	 * @date 28/Sep/2013
	 */
	struct MenuRateMuonScaling : public l1menu::IMenuRate
	{
	public:
		MenuRateMuonScaling( std::shared_ptr<const l1menu::IMenuRate> pUnscaledMenuRate, const std::string& muonScalingFilename, const l1menu::MenuFitter& fitter );
		virtual ~MenuRateMuonScaling();

		// The methods required by the IMenuRate interface
		virtual float totalFraction() const;
		virtual float totalFractionError() const;
		virtual float totalRate() const;
		virtual float totalRateError() const;

		virtual const std::vector<const l1menu::ITriggerRate*>& triggerRates() const;
	private:
		std::unique_ptr<class MenuRateMuonScalingPrivateMembers> pImple_;
	};

} // end of namespace l1menu

#endif
