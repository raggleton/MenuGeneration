#ifndef l1menu_OnlineToOfflineScaling_h
#define l1menu_OnlineToOfflineScaling_h

#include "l1menu/IScaling.h"
#include <memory>

namespace l1menu
{
	namespace scalings
	{
		/** @brief Implementation of IScaling that scales the online thresholds to offline thresholds.
		 *
		 * The scalings this class provides are arbitrary depending on the input file, but the offline
		 * thresholds are generally the 95% efficient thresholds (or ??% for taus, I think 85%).
		 *
		 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
		 * @date 29/Sep/2013
		 */
		class OnlineToOfflineScaling : public l1menu::IScaling
		{
		public:
			OnlineToOfflineScaling( const std::string& offlineScalingFilename );

			virtual ~OnlineToOfflineScaling();
			virtual std::string briefDescription();
			virtual std::string detailedDescription();
			virtual std::unique_ptr<l1menu::TriggerRatePlot> scale( const l1menu::TriggerRatePlot& unscaledPlot );
			virtual std::unique_ptr<l1menu::MenuRatePlots> scale( const l1menu::MenuRatePlots& unscaledPlots );
			virtual std::unique_ptr<l1menu::IMenuRate> scale( const l1menu::IMenuRate& unscaledMenuRate );
		private:
			std::unique_ptr<class OnlineToOfflineScalingPrivateMembers> pImple;
		};

	} // end of namespace scalings
} // end of namespace l1menu

#endif
