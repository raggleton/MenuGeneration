#include "HTM.h"

#include <stdexcept>
#include "../implementation/RegisterTriggerMacro.h"
#include "l1menu/L1TriggerDPGEvent.h"
#include "L1AnalysisDataFormat.h"


namespace l1menu
{
	namespace triggers
	{

		/* The REGISTER_TRIGGER macro will make sure that the given trigger is registered in the
		 * l1menu::TriggerTable when the program starts. I also want to provide some suggested binning
		 * however. The REGISTER_TRIGGER_AND_CUSTOMISE macro does exactly the same but lets me pass
		 * a pointer to a function that will be called directly after the trigger has been registered
		 * at program startup. The function takes no parameters and returns void. In this case I'm
		 * giving it a lambda function.
		 */
		REGISTER_TRIGGER_AND_CUSTOMISE( HTM_v0,
			[]() // Use a lambda function to customise rather than creating a named function that never gets used again.
			{
				l1menu::TriggerTable& triggerTable=l1menu::TriggerTable::instance();
				HTM_v0 tempTriggerInstance;
				triggerTable.registerSuggestedBinning( tempTriggerInstance.name(), "threshold1", 100, 0, 200 );
			} // End of customisation lambda function
		) // End of REGISTER_TRIGGER_AND_CUSTOMISE macro call

	} // end of namespace triggers

} // end of namespace l1menu


//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//---------------  Definitions below         ---------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------


bool l1menu::triggers::HTM_v0::apply( const l1menu::L1TriggerDPGEvent& event ) const
{
	const L1Analysis::L1AnalysisDataFormat& analysisDataFormat=event.rawEvent();
	const bool* PhysicsBits=event.physicsBits();

	bool raw = PhysicsBits[0];   // ZeroBias
	if (! raw) return false;

	float adc = analysisDataFormat.HTM ;
	float TheHTM = adc; // / 2. ;

	if (TheHTM < threshold1_) return false;
	return true;
}

bool l1menu::triggers::HTM_v0::thresholdsAreCorrelated() const
{
	return false;
}

unsigned int l1menu::triggers::HTM_v0::version() const
{
	return 0;
}

l1menu::triggers::HTM::HTM()
	: threshold1_(50)
{
	// No operation other than the initialiser list
}

const std::string l1menu::triggers::HTM::name() const
{
	return "L1_HTM";
}

const std::vector<std::string> l1menu::triggers::HTM::parameterNames() const
{
	std::vector<std::string> returnValue;
	returnValue.push_back("threshold1");
	return returnValue;
}

float& l1menu::triggers::HTM::parameter( const std::string& parameterName )
{
	if( parameterName=="threshold1" ) return threshold1_;
	else throw std::logic_error( "Not a valid parameter name" );
}

const float& l1menu::triggers::HTM::parameter( const std::string& parameterName ) const
{
	if( parameterName=="threshold1" ) return threshold1_;
	else throw std::logic_error( "Not a valid parameter name" );
}
