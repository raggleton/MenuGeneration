#include "DoubleMu.h"

#include "../implementation/RegisterTriggerMacro.h"

namespace l1menu
{
	namespace triggers
	{

		/** @brief Essentially a copy of the DoubleMu trigger, but overriding the name, since we
		 * currently have no isolation on the muons.
		 *
		 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
		 * @date 04/Jun/2013
		 */
		class IsoMu_Mu_v0 : public l1menu::triggers::DoubleMu_v0
		{
		public:
			virtual const std::string name() const;
			virtual unsigned int version() const;
		}; // end of version 0 class

		/* The REGISTER_TRIGGER macro will make sure that the given trigger is registered in the
		 * l1menu::TriggerTable when the program starts. I also want to provide some suggested binning
		 * however. The REGISTER_TRIGGER_AND_CUSTOMISE macro does exactly the same but lets me pass
		 * a pointer to a function that will be called directly after the trigger has been registered
		 * at program startup. The function takes no parameters and returns void. In this case I'm
		 * giving it a lambda function.
		 */
		REGISTER_TRIGGER_AND_CUSTOMISE( IsoMu_Mu_v0,
			[]() // Use a lambda function to customise rather than creating a named function that never gets used again.
			{
				l1menu::TriggerTable& triggerTable=l1menu::TriggerTable::instance();
				IsoMu_Mu_v0 tempTriggerInstance;
				triggerTable.registerSuggestedBinning( tempTriggerInstance.name(), "threshold1", 100, 0, 100 );
				triggerTable.registerSuggestedBinning( tempTriggerInstance.name(), "threshold2", 100, 0, 100 );
			} // End of customisation lambda function
		) // End of REGISTER_TRIGGER_AND_CUSTOMISE macro call

	} // end of namespace triggers

} // end of namespace l1menu

const std::string l1menu::triggers::IsoMu_Mu_v0::name() const
{
	return "L1_isoMu_Mu";
}

unsigned int l1menu::triggers::IsoMu_Mu_v0::version() const
{
	return 0;
}
