#ifndef l1menu_MenuRateOfflineScaling_h
#define l1menu_MenuRateOfflineScaling_h

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
}

namespace l1menu
{
	/** @brief Applies scalings to change the threshold from online to offline
	 *
	 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
	 * @date 29/Sep/2013
	 */
	struct MenuRateOfflineScaling : public l1menu::IMenuRate
	{
	public:
		MenuRateOfflineScaling( std::shared_ptr<const l1menu::IMenuRate> pUnscaledMenuRate, const std::string& offlineScalingFilename );
		virtual ~MenuRateOfflineScaling();

		// The methods required by the IMenuRate interface
		virtual float totalFraction() const;
		virtual float totalFractionError() const;
		virtual float totalRate() const;
		virtual float totalRateError() const;

		virtual const std::vector<const l1menu::ITriggerRate*>& triggerRates() const;
	private:
		std::unique_ptr<class MenuRateOfflineScalingPrivateMembers> pImple_;
	};

} // end of namespace l1menu

#endif
