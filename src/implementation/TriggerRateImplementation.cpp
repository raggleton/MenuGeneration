#include "TriggerRateImplementation.h"

#include <cmath>
#include "MenuRateImplementation.h"
#include "l1menu/TriggerTable.h"
#include "l1menu/ITrigger.h"

l1menu::implementation::TriggerRateImplementation::TriggerRateImplementation( const l1menu::ITrigger& trigger, float weightOfEventsPassingThisTrigger, float weightSquaredOfEventsPassingThisTrigger, float weightOfEventsOnlyPassingThisTrigger, float weightSquaredOfEventsOnlyPassingThisTrigger, const MenuRateImplementation& menuRate )
	: weightOfEventsPassingThisTrigger_(weightOfEventsPassingThisTrigger),
	  weightSquaredOfEventsPassingThisTrigger_(weightSquaredOfEventsPassingThisTrigger),
	  weightOfEventsOnlyPassingThisTrigger_(weightOfEventsOnlyPassingThisTrigger),
	  weightSquaredOfEventsOnlyPassingThisTrigger_(weightSquaredOfEventsOnlyPassingThisTrigger),
	  pMenuRate_(&menuRate)
{
	pTrigger_=std::move( l1menu::TriggerTable::instance().copyTrigger(trigger) );
}

l1menu::implementation::TriggerRateImplementation::TriggerRateImplementation( TriggerRateImplementation&& otherTriggerRate ) noexcept
	: pTrigger_( std::move(otherTriggerRate.pTrigger_) ),
	  weightOfEventsPassingThisTrigger_(otherTriggerRate.weightOfEventsPassingThisTrigger_),
	  weightSquaredOfEventsPassingThisTrigger_(otherTriggerRate.weightSquaredOfEventsPassingThisTrigger_),
	  weightOfEventsOnlyPassingThisTrigger_(otherTriggerRate.weightOfEventsOnlyPassingThisTrigger_),
	  weightSquaredOfEventsOnlyPassingThisTrigger_(otherTriggerRate.weightSquaredOfEventsOnlyPassingThisTrigger_),
	  pMenuRate_(otherTriggerRate.pMenuRate_)
{
	// No operation besides the initialiser list
}

l1menu::implementation::TriggerRateImplementation& l1menu::implementation::TriggerRateImplementation::operator=( TriggerRateImplementation&& otherTriggerRate ) noexcept
{
	pTrigger_=std::move( otherTriggerRate.pTrigger_ );
	weightOfEventsPassingThisTrigger_=otherTriggerRate.weightOfEventsPassingThisTrigger_;
	weightSquaredOfEventsPassingThisTrigger_=otherTriggerRate.weightSquaredOfEventsPassingThisTrigger_;
	weightOfEventsOnlyPassingThisTrigger_=otherTriggerRate.weightOfEventsOnlyPassingThisTrigger_;
	weightSquaredOfEventsOnlyPassingThisTrigger_=otherTriggerRate.weightSquaredOfEventsOnlyPassingThisTrigger_;
	pMenuRate_=otherTriggerRate.pMenuRate_;
	return *this;
}

l1menu::implementation::TriggerRateImplementation::~TriggerRateImplementation()
{
	// No operation
}

const l1menu::ITrigger& l1menu::implementation::TriggerRateImplementation::trigger() const
{
	return *pTrigger_;
}

float l1menu::implementation::TriggerRateImplementation::fraction() const
{
	return weightOfEventsPassingThisTrigger_/pMenuRate_->weightOfAllEvents();
}

float l1menu::implementation::TriggerRateImplementation::fractionError() const
{
	return std::sqrt(weightSquaredOfEventsPassingThisTrigger_)/pMenuRate_->weightOfAllEvents();
}

float l1menu::implementation::TriggerRateImplementation::rate() const
{
	return fraction()*pMenuRate_->scaling();
}

float l1menu::implementation::TriggerRateImplementation::rateError() const
{
	return fractionError()*pMenuRate_->scaling();
}

float l1menu::implementation::TriggerRateImplementation::pureFraction() const
{
	return weightOfEventsOnlyPassingThisTrigger_/pMenuRate_->weightOfAllEvents();
}

float l1menu::implementation::TriggerRateImplementation::pureFractionError() const
{
	return std::sqrt(weightSquaredOfEventsOnlyPassingThisTrigger_)/pMenuRate_->weightOfAllEvents();
}

float l1menu::implementation::TriggerRateImplementation::pureRate() const
{
	return pureFraction()*pMenuRate_->scaling();
}

float l1menu::implementation::TriggerRateImplementation::pureRateError() const
{
	return pureFractionError()*pMenuRate_->scaling();
}
