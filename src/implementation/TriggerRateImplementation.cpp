#include "TriggerRateImplementation.h"

#include <cmath>
#include <stdexcept>
#include "MenuRateImplementation.h"
#include "l1menu/TriggerTable.h"
#include "l1menu/ITrigger.h"
#include "l1menu/tools/XMLElement.h"
#include "l1menu/tools/fileIO.h"

l1menu::implementation::TriggerRateImplementation::TriggerRateImplementation( const l1menu::ITrigger& trigger, float fraction, float fractionError, float rate, float rateError, float pureFraction, float pureFractionError, float pureRate, float pureRateError )
	: fraction_(fraction), fractionError_(fractionError),
	  rate_(rate), rateError_(rateError),
	  pureFraction_(pureFraction), pureFractionError_(pureFractionError),
	  pureRate_(pureRate), pureRateError_(pureRateError)
{
	pTrigger_=std::move( l1menu::TriggerTable::instance().copyTrigger(trigger) );
}

l1menu::implementation::TriggerRateImplementation::TriggerRateImplementation( TriggerRateImplementation&& otherTriggerRate ) noexcept
	: pTrigger_( std::move(otherTriggerRate.pTrigger_) ),
	  parameterErrorsHigh_( std::move(otherTriggerRate.parameterErrorsHigh_) ),
	  parameterErrorsLow_( std::move(otherTriggerRate.parameterErrorsLow_) ),
	  fraction_(otherTriggerRate.fraction_),
	  fractionError_(otherTriggerRate.fractionError_),
	  rate_(otherTriggerRate.rate_),
	  rateError_(otherTriggerRate.rateError_),
	  pureFraction_(otherTriggerRate.pureFraction_),
	  pureFractionError_(otherTriggerRate.pureFractionError_),
	  pureRate_(otherTriggerRate.pureRate_),
	  pureRateError_(otherTriggerRate.pureRateError_)
{
	// No operation besides the initialiser list
}

l1menu::implementation::TriggerRateImplementation& l1menu::implementation::TriggerRateImplementation::operator=( TriggerRateImplementation&& otherTriggerRate ) noexcept
{
	pTrigger_=std::move( otherTriggerRate.pTrigger_ );
	parameterErrorsHigh_=std::move(otherTriggerRate.parameterErrorsHigh_);
	parameterErrorsLow_=std::move(otherTriggerRate.parameterErrorsLow_);
	fraction_=otherTriggerRate.fraction_;
	fractionError_=otherTriggerRate.fractionError_;
	rate_=otherTriggerRate.rate_;
	rateError_=otherTriggerRate.rateError_;
	pureFraction_=otherTriggerRate.pureFraction_;
	pureFractionError_=otherTriggerRate.pureFractionError_;
	pureRate_=otherTriggerRate.pureRate_;
	pureRateError_=otherTriggerRate.pureRateError_;
	return *this;
}

l1menu::implementation::TriggerRateImplementation::~TriggerRateImplementation()
{
	// No operation
}

void l1menu::implementation::TriggerRateImplementation::setParameterErrors( const std::string& parameterName, float errorLow, float errorHigh )
{
	// Get the parmeter from the trigger just so that I can make sure the name is valid.
	// This call will throw an exception if it's not.
	pTrigger_->parameter( parameterName );
	parameterErrorsLow_[parameterName]=errorLow;
	parameterErrorsHigh_[parameterName]=errorHigh;
}

const l1menu::ITriggerDescription& l1menu::implementation::TriggerRateImplementation::trigger() const
{
	return *pTrigger_;
}

bool l1menu::implementation::TriggerRateImplementation::parameterErrorsAreAvailable( const std::string& parameterName ) const
{
	// I only need to check one of the maps. If it's in one it has to also be in the other.
	const auto& iFindResult=parameterErrorsLow_.find(parameterName);
	if( iFindResult==parameterErrorsLow_.end() ) return false;
	return true;
}

const float& l1menu::implementation::TriggerRateImplementation::parameterErrorLow( const std::string& parameterName ) const
{
	const auto& iFindResult=parameterErrorsLow_.find(parameterName);
	if( iFindResult==parameterErrorsLow_.end() ) throw std::runtime_error( "TriggerRateImplementation::parameterErrorLow - No error set for parameter "+parameterName);
	return iFindResult->second;
}

const float& l1menu::implementation::TriggerRateImplementation::parameterErrorHigh( const std::string& parameterName ) const
{
	const auto& iFindResult=parameterErrorsHigh_.find(parameterName);
	if( iFindResult==parameterErrorsHigh_.end() ) throw std::runtime_error( "TriggerRateImplementation::parameterErrorHigh - No error set for parameter "+parameterName);
	return iFindResult->second;
}

float l1menu::implementation::TriggerRateImplementation::fraction() const
{
	return fraction_;
}

float l1menu::implementation::TriggerRateImplementation::fractionError() const
{
	return fractionError_;
}

float l1menu::implementation::TriggerRateImplementation::rate() const
{
	return rate_;
}

float l1menu::implementation::TriggerRateImplementation::rateError() const
{
	return rateError_;
}

float l1menu::implementation::TriggerRateImplementation::pureFraction() const
{
	return pureFraction_;
}

float l1menu::implementation::TriggerRateImplementation::pureFractionError() const
{
	return pureFractionError_;
}

float l1menu::implementation::TriggerRateImplementation::pureRate() const
{
	return pureRate_;
}

float l1menu::implementation::TriggerRateImplementation::pureRateError() const
{
	return pureRateError_;
}
