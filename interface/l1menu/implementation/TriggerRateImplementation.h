#ifndef l1menu_implementation_TriggerRateImplementation_h
#define l1menu_implementation_TriggerRateImplementation_h

#include "l1menu/ITriggerRate.h"
#include <vector>
#include <memory>

//
// Forward declarations
//
namespace l1menu
{
	class ITrigger;
	namespace implementation
	{
		class MenuRateImplementation;
	}
}


namespace l1menu
{
	namespace implementation
	{
		/** @brief Implementation of the ITriggerRate interface.
		 *
		 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
		 * @date 28/Jun/2013
		 */
		class TriggerRateImplementation : public l1menu::ITriggerRate
		{
		public:
			TriggerRateImplementation( const l1menu::ITrigger& trigger, float weightOfEventsPassingThisTrigger, float weightOfEventsOnlyPassingThisTrigger, const MenuRateImplementation& menuRate );
			TriggerRateImplementation& operator=( TriggerRateImplementation&& otherTriggerRate ); // Move assignment
			virtual ~TriggerRateImplementation();

			// Methods required by the l1menu::ITriggerRate interface
			virtual const l1menu::ITrigger& trigger() const;
			virtual float fraction() const;
			virtual float rate() const;
			virtual float pureFraction() const;
			virtual float pureRate() const;
		protected:
			std::unique_ptr<l1menu::ITrigger> pTrigger_;
			float weightOfEventsPassingThisTrigger_;
			float weightOfEventsOnlyPassingThisTrigger_;
			const MenuRateImplementation& menuRate_;
		};


	} // end of the implementation namespace
} // end of the l1menu namespace
#endif
