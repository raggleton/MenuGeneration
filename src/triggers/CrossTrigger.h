#ifndef l1menu_triggers_CrossTrigger_h
#define l1menu_triggers_CrossTrigger_h

#include <string>
#include <vector>
#include <memory>
#include "l1menu/ITrigger.h"

namespace l1menu
{
	namespace triggers
	{
		/** @brief Abstract base class for cross triggers.
		 *
		 * This is abstract because it doesn't implement the name and version methods
		 * from the l1menu::ITrigger interface. These should be provided by the derived
		 * class.
		 *
		 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
		 * @date 03/Jun/2013
		 */
		class CrossTrigger : public l1menu::ITrigger
		{
		public:
			CrossTrigger( std::unique_ptr<l1menu::ITrigger> pLeg1Trigger, std::unique_ptr<l1menu::ITrigger> pLeg2Trigger );
			/** @brief Constructor using basic pointers. Note that this class takes ownership. */
			CrossTrigger( l1menu::ITrigger* pLeg1Trigger, l1menu::ITrigger* pLeg2Trigger );
			virtual ~CrossTrigger();
			virtual const std::vector<std::string> parameterNames() const;
			virtual float& parameter( const std::string& parameterName );
			virtual const float& parameter( const std::string& parameterName ) const;
			virtual bool apply( const l1menu::L1TriggerDPGEvent& event ) const;
		protected:
			std::unique_ptr<l1menu::ITrigger> pLeg1_;
			std::unique_ptr<l1menu::ITrigger> pLeg2_;
		};

	} // end of namespace triggers

} // end of namespace l1menu

#endif
