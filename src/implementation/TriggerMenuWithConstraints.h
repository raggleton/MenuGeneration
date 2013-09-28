#ifndef l1menu_implementation_TriggerMenuWithConstraints_h
#define l1menu_implementation_TriggerMenuWithConstraints_h

#include "l1menu/TriggerMenu.h"
#include "l1menu/TriggerRatePlot.h"

//
// Forward declarations
//
namespace l1menu
{
	class MenuRatePlots;
	class ISample;
}

namespace l1menu
{
	namespace implementation
	{
		/** @brief Extension of the TriggerMenu class that also stores some constraints used for fitting menus
		 *
		 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
		 * @date 27/Sep/2013
		 */
		class TriggerMenuWithConstraints : public l1menu::TriggerMenu
		{
		public:
			/** @brief Structure to collate a few things about triggers that can be scaled
			 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
			 * @date 08/Jul/2013
			 */
			struct TriggerScalingDetails
			{
				size_t triggerNumber; ///< The number of the trigger in the trigger table
				float bandwidthFraction; ///< The fraction of the total bandwidth requested for this trigger
				std::unique_ptr<l1menu::TriggerRatePlot> pRatePlot; ///< The rate plot for this trigger. Not set by default so often a nullptr.
				std::string mainThreshold;
				std::vector< std::pair<std::string,float> > thresholdScalings; ///< The constant to scale each threshold compared to the main threshold
			};
		public:
			virtual ~TriggerMenuWithConstraints();
			virtual void loadMenuFromFile( const std::string& filename );
			bool findAndCopyTriggerRatePlot( const l1menu::MenuRatePlots& menuRatePlots, size_t triggerNumber );
			bool createTriggerRatePlot( const l1menu::ISample& sample, size_t triggerNumber );
			const TriggerMenuWithConstraints::TriggerScalingDetails& scalingDetails( size_t triggerNumber );
		protected:
			std::vector<TriggerMenuWithConstraints::TriggerScalingDetails> scalableTriggers;
		};


	} // end of the implementation namespace
} // end of the l1menu namespace
#endif
