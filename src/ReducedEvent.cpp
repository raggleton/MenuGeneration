#include "l1menu/ReducedEvent.h"

#include "l1menu/ITrigger.h"
#include "l1menu/ReducedSample.h"
#include "protobuf/l1menu.pb.h"

l1menu::ReducedEvent::ReducedEvent( const l1menu::ReducedSample& sample )
	: sample_(sample)
{
	// No operation
}

l1menu::ReducedEvent::~ReducedEvent()
{
	// No operation
}

float l1menu::ReducedEvent::parameterValue( ParameterID parameterNumber ) const
{
	return pProtobufEvent_->threshold(parameterNumber);
}

bool l1menu::ReducedEvent::passesTrigger( const l1menu::ITrigger& trigger ) const
{
	const auto& parameterIdentifiers=sample_.getTriggerParameterIdentifiers(trigger);

	for( const auto& identifier : parameterIdentifiers )
	{
		if( trigger.parameter(identifier.first)>parameterValue(identifier.second) ) return false;
	}

	// If control got this far, all of the thresholds passed.
	return true;
}

float l1menu::ReducedEvent::weight() const
{
	if( pProtobufEvent_->has_weight() ) return pProtobufEvent_->weight();
	else return 1;
}

const l1menu::ISample& l1menu::ReducedEvent::sample() const
{
	return sample_;
}
