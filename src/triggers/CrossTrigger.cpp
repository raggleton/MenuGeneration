#include "CrossTrigger.h"

#include <stdexcept>

l1menu::triggers::CrossTrigger::CrossTrigger( std::unique_ptr<l1menu::ITrigger> pLeg1, std::unique_ptr<l1menu::ITrigger> pLeg2 )
: pLeg1_( std::move(pLeg1) ), pLeg2_( std::move(pLeg2) )
{
	// No operation besides the initialiser list
}

l1menu::triggers::CrossTrigger::CrossTrigger( l1menu::ITrigger* pLeg1, l1menu::ITrigger* pLeg2 )
: pLeg1_( pLeg1 ), pLeg2_( pLeg2 )
{
	// No operation besides the initialiser list
}

l1menu::triggers::CrossTrigger::~CrossTrigger()
{
	// No operation
}

const std::vector<std::string> l1menu::triggers::CrossTrigger::parameterNames() const
{
	std::vector<std::string> returnValue;

	std::vector<std::string> legParameterNames=pLeg1_->parameterNames();
	for( const auto& parameterName : legParameterNames ) returnValue.push_back( "leg1"+parameterName );

	legParameterNames=pLeg2_->parameterNames();
	for( const auto& parameterName : legParameterNames ) returnValue.push_back( "leg2"+parameterName );

	return returnValue;
}

float& l1menu::triggers::CrossTrigger::parameter( const std::string& parameterName )
{
	std::string firstFourCharacters=parameterName.substr(0,4);
	std::string everythingElse=parameterName.substr(4);

	if( firstFourCharacters=="leg1" ) return pLeg1_->parameter(everythingElse);
	else if( firstFourCharacters=="leg2" ) return pLeg2_->parameter(everythingElse);
	else throw std::logic_error( "Not a valid parameter name (\""+parameterName+"\")" );
}

const float& l1menu::triggers::CrossTrigger::parameter( const std::string& parameterName ) const
{
	std::string firstFourCharacters=parameterName.substr(0,4);
	std::string everythingElse=parameterName.substr(4);

	if( firstFourCharacters=="leg1" ) return pLeg1_->parameter(everythingElse);
	else if( firstFourCharacters=="leg2" ) return pLeg2_->parameter(everythingElse);
	else throw std::logic_error( "Not a valid parameter name (\""+parameterName+"\")" );
}

bool l1menu::triggers::CrossTrigger::apply( const l1menu::L1TriggerDPGEvent& event ) const
{
	return pLeg1_->apply(event) && pLeg2_->apply(event);
}

bool l1menu::triggers::CrossTrigger::thresholdsAreCorrelated() const
{
	// If any thresholds in either of the legs are correlated then the say the whole trigger is
	return pLeg1_->thresholdsAreCorrelated() || pLeg2_->thresholdsAreCorrelated();
}
