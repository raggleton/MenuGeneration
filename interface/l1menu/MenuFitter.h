#ifndef l1menu_MenuFitter_h
#define l1menu_MenuFitter_h

#include <memory>

// Forward declarations
namespace l1menu
{
	class ISample;
	class ITrigger;
	class TriggerMenu;
	class IMenuRate;
	class MenuRatePlots;
}

namespace l1menu
{
	/** @brief Changes the thresholds in a trigger menu to provide a given rate.
	 *
	 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
	 * @date 07/Jul/2013
	 */
	class MenuFitter
	{
	public:
		MenuFitter( const l1menu::ISample& sample );
		MenuFitter( const l1menu::ISample& sample, const l1menu::MenuRatePlots& menuRatePlots );
		virtual ~MenuFitter();
		const l1menu::TriggerMenu& menu() const;
		std::unique_ptr<const l1menu::IMenuRate> fit( float totalRate, float tolerance );
		void addTrigger( const l1menu::ITrigger& trigger, float fractionOfTotalBandwidth, bool lockThresholds=false );
		void loadMenuFromFile( const std::string& filename );
	private:
		std::unique_ptr<class MenuFitterPrivateMembers> pImple_;
	};

}
#endif
