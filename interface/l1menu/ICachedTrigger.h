#ifndef l1menu_ICachedTrigger_h
#define l1menu_ICachedTrigger_h

//
// Forward declarations
//
namespace l1menu
{
	class IEvent;
}


namespace l1menu
{
	/** @brief An interface to a proxy trigger object that can be requested from ISamples that *may* make
	 * processing significantly faster.
	 *
	 * Some implementations of ISample can cache some information in this object that helps them
	 * process trigger rates etcetera significantly faster. For other implementations there is no
	 * improvement.
	 *
	 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
	 * @date 26/Jun/2013
	 */
	class ICachedTrigger
	{
	public:
		virtual ~ICachedTrigger() {}
		/** @brief Whether or not the event passes this trigger. */
		virtual bool apply( const l1menu::IEvent& event ) = 0;
	}; // end of class ICachedTrigger

} // end of namespace l1menu

#endif
