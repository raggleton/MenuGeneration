#ifndef l1menu_ITrigger_h
#define l1menu_ITrigger_h

#include <string>
#include <vector>

// Forward declarations
namespace l1menu
{
	class L1TriggerDPGEvent;
}


namespace l1menu
{
	/** @brief Abstract interface for triggers used for menu studies.
	 *
	 * A trigger should be uniquely distinguished by its name and version. The version
	 * just allows updates to be tested alongside previous versions.
	 *
	 * Thresholds are all stored as named parameters. The current naming convention is
	 * "threshold1", and if there are any more thresholds they are "threshold2" etcetera.
	 * For multi-leg triggers the name is prepended with "leg1" or "leg2", so for example
	 * it could be "leg1threshold1", "leg1threshold2" and "leg2threshold1" for a multi-leg
	 * trigger that has two thresholds on the first leg and one on the second. Of course
	 * this depends on the implementation of this interface to follow the convention.
	 * There are some tools in l1menu::tools to get a std::vector of the threshold names.
	 *
	 * Other parameters can be set and queried in the same way, depending on the
	 * implementation. Current examples are "muonQuality", "etaCut" and "regionCut". Muon
	 * triggers tend to specify eta cuts in absolute eta ("etaCut"), whereas jets and
	 * e-gamma use calorimeter region ("regionCut"). There are some tools in l1menu::tools
	 * to convert between the two.
	 *
	 * Any implementation of ISample and ITrigger should be able to work together, so
	 * any trigger implementation should be able to run on any sample file format.
	 * The most low level way of seeing if a trigger passes an event is to get an
	 * l1menu::L1TriggerDPGEvent from an l1menu::FullSample and calling apply().
	 * To use the ISample implementation agnostic way, get an IEvent from the ISample,
	 * and pass the trigger to IEvent::passesTrigger(). That delegates to however the
	 * ISample/IEvent implementation wants to do it.
	 *
	 * For details on implementing new triggers by subclassing this interface, see
	 * @ref L1Trigger_MenuGeneration_implementingTriggers.
	 *
	 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
	 * @date sometime around June 2013
	 */
	class ITrigger
	{
	public:
		virtual ~ITrigger() {}
		virtual const std::string name() const = 0;
		virtual unsigned int version() const = 0;
		virtual const std::vector<std::string> parameterNames() const = 0;
		virtual float& parameter( const std::string& parameterName ) = 0;
		virtual const float& parameter( const std::string& parameterName ) const = 0;
		virtual bool apply( const l1menu::L1TriggerDPGEvent& event ) const = 0;
	};

} // end of namespace l1menu

#endif
