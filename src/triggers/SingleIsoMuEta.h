#ifndef l1menu_triggers_SingleIsoMuEta_h
#define l1menu_triggers_SingleIsoMuEta_h

#include "SingleMuEta.h"

namespace l1menu
{
	namespace triggers
	{

		/** @brief Essentially a copy of the SingleMuEta trigger, but overriding the name, since we
		 * currently have no isolation on the muons.
		 *
		 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
		 * @date 04/Jun/2013
		 */
		class SingleIsoMuEta_v0 : public l1menu::triggers::SingleMuEta_v0
		{
		public:
			virtual const std::string name() const;
			virtual unsigned int version() const;
		}; // end of version 0 class

	} // end of namespace triggers

} // end of namespace l1menu

#endif
