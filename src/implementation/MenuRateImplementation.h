#ifndef l1menu_implementation_MenuRateImplementation_h
#define l1menu_implementation_MenuRateImplementation_h

#include "l1menu/IMenuRate.h"
#include <vector>
#include "TriggerRateImplementation.h"

//
// Forward declarations
//
namespace l1menu
{
	class ITrigger;
	class ITriggerRate;
	class TriggerMenu;
	class ISample;
}


namespace l1menu
{
	namespace implementation
	{
		/** @brief Implementation of the IMenuRate interface.
		 *
		 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
		 * @date 28/Jun/2013
		 */
		class MenuRateImplementation : public l1menu::IMenuRate
		{
		public:
			MenuRateImplementation( const l1menu::TriggerMenu& menu, const l1menu::ISample& sample );
			float weightOfAllEvents() const;
			float weightOfAllEventsPassingAnyTrigger() const;
			void setWeightOfAllEvents( float weightOfAllEvents );
			void setWeightOfEventsPassingAnyTrigger( float weightOfEventsPassingAnyTrigger );

			void setScaling( float scaling );
			float scaling() const;
			void addTriggerRate( const l1menu::ITrigger& trigger, float weightOfEventsPassingTheTrigger, float weightOfEventsOnlyPassingTheTrigger );
			// Methods required by the l1menu::IMenuRate interface
			virtual float totalFraction() const;
			virtual float totalRate() const;
			virtual const std::vector<const l1menu::ITriggerRate*>& triggerRates() const;
		protected:
			float weightOfAllEvents_;
			float weightOfEventsPassingAnyTrigger_;
			float scaling_;
			std::vector<TriggerRateImplementation> triggerRates_;
		private:
			mutable std::vector<const l1menu::ITriggerRate*> baseClassReferences_;
		};


	} // end of the implementation namespace
} // end of the l1menu namespace
#endif
