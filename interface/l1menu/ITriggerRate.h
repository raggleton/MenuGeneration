#ifndef l1menu_ITriggerRate_h
#define l1menu_ITriggerRate_h

#include <vector>

//
// Forward declarations
//
namespace l1menu
{
	class ITrigger;
}

namespace l1menu
{
	/** @brief Interface to get information about the trigger rate. Read only.
	 *
	 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
	 * @date 24/Jun/2013
	 */
	class ITriggerRate
	{
	public:
		virtual ~ITriggerRate() {}

		/** @brief The trigger that gives the rate, which can be queiried for thresholds etcetera
		 * N.B. This trigger is a copy of whatever was used to calculate the rate. Changing one will
		 * have no affect on the other.
		 */
		virtual const l1menu::ITrigger& trigger() const = 0;

		/** @brief The fraction of events that this trigger passed, so before applying any scaling. */
		virtual float fraction() const = 0;

		/** @brief The rate, so fraction multiplied by the scaling. */
		virtual float rate() const = 0;

		/** @brief The fraction of events that pass only this trigger, with no scaling. */
		virtual float pureFraction() const = 0;

		/** @brief The pure rate, so pureFraction multiplied by the scaling. */
		virtual float pureRate() const = 0;
	};

} // end of namespace l1menu

#endif
