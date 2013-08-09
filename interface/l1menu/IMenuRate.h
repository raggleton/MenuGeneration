#ifndef l1menu_IMenuRate_h
#define l1menu_IMenuRate_h

#include <vector>

//
// Forward declarations
//
namespace l1menu
{
	class ITriggerRate;
}

namespace l1menu
{
	/** @brief Interface to the rates for a collection; individually, total and (eventually) correlations.
	 *
	 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
	 * @date 24/Jun/2013
	 */
	struct IMenuRate
	{
	public:
		virtual ~IMenuRate() {}

		/** @brief The fraction of events that passed all triggers, 1 being all events and 0 no events. */
		virtual float totalFraction() const = 0;
		virtual float totalRate() const = 0;

		virtual const std::vector<const l1menu::ITriggerRate*>& triggerRates() const = 0;
	};

} // end of namespace l1menu

#endif
