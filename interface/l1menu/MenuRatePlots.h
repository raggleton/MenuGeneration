#ifndef l1menu_MenuRatePlots_h
#define l1menu_MenuRatePlots_h

#include <vector>
#include <cstddef> // required to define NULL

#include <l1menu/TriggerRatePlot.h>

//
// Forward declarations
//
class TDirectory;
class TH1;
namespace l1menu
{
	class TriggerMenu;
}


namespace l1menu
{
	/** @brief Class that makes rate versus threshold plots for each of the triggers in the given menu.
	 *
	 * Note that when an instance goes out of scope the root histograms it has created are deleted, unless
	 * releaseAllPlots() is called before hand.
	 *
	 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
	 * @date 24/May/2013
	 */
	class MenuRatePlots
	{
	public:
		/** @brief Sets the triggers that plots should be made for and the directory to put them in.
		 *
		 * @param[in] triggerMenu The trigger menu with each of the triggers plots are required for. Any relevant
		 *                        parameters for each trigger (e.g. eta cuts) should be set before this is called.
		 *                        A reference to the menu is not kept so changes to the menu and/or triggers will
		 *                        have no effect after this constructor is called.
		 * @param[in] pDirectory  A pointer to the root TDirectory where the plots should be created. Plots are
		 *                        created directly inside this directory so it's advised this is empty to avoid
		 *                        name clashes. Defaults to NULL which means the histograms will be held in memory
		 *                        only.
		 */
		MenuRatePlots( const l1menu::TriggerMenu& triggerMenu, TDirectory* pDirectory=NULL );

		explicit MenuRatePlots( const TDirectory* pPreExistingPlotDirectory );

		void addEvent( const l1menu::IEvent& event );

		void addSample( const l1menu::ISample& sample );

		/** @brief Set the root TDirectory where the histograms will reside. */
		void setDirectory( TDirectory* pDirectory );

		std::vector<TH1*> getPlots();

		/** @brief Relinquish ownership of all the root TH1 plots.
		 *
		 * If this is not called the plots will be deleted when this instance goes out of scope. The
		 * pointers are still held however, so operations like addEvent() are still possible.*/
		void relinquishOwnershipOfPlots();
	protected:
		std::vector<l1menu::TriggerRatePlot> triggerPlots_;
	};
}
#endif
