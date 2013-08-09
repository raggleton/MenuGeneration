#ifndef l1menu_ReducedEvent_h
#define l1menu_ReducedEvent_h

#include "l1menu/IEvent.h"
#include <stddef.h> // required for size_t

//
// Forward declarations
//
namespace l1menu
{
	class ITrigger;
	class ReducedSample;
}
namespace l1menuprotobuf
{
	class Event;
}


namespace l1menu
{
	/** @brief Interface for a simplified event format. The event just has the minimum threshold to pass for each trigger recorded.
	 *
	 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
	 * @date 28/May/2013
	 */
	class ReducedEvent : public l1menu::IEvent
	{
		friend class l1menu::ReducedSample;
	public:
		typedef size_t ParameterID;
	public:
		ReducedEvent( const l1menu::ReducedSample& sample );
		virtual ~ReducedEvent();
		virtual float parameterValue( ParameterID parameterNumber ) const;

		//
		// These are the methods required by the l1menu::IEvent interface.
		//
		virtual bool passesTrigger( const l1menu::ITrigger& trigger ) const;
		virtual float weight() const;
		virtual const l1menu::ISample& sample() const;
	private:
		l1menuprotobuf::Event* pProtobufEvent_;
		const l1menu::ReducedSample& sample_; ///< @brief The sample that this event is from
	};

} // end of namespace l1menu


#endif
