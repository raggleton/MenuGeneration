// I need to set a #define before including this header so that I don't get the
// "extern" instantiation. This should be done in this file only, because I will
// create the full instantiation in this file. There's a comment in the header
// about this.

//#define WILL_INSTANTIATE_l1menu_tools_conversionToXML_h
#include "l1menu/tools/conversionToXML.h"
//#undef WILL_INSTANTIATE_l1menu_tools_conversionToXML_h

#include "l1menu/TriggerMenu.h"


namespace l1menu
{
	namespace tools
	{
		template<> void convertToXML<l1menu::TriggerMenu>( const l1menu::TriggerMenu& object, l1menu::tools::XMLElement& parent );
	}
}


template<> void l1menu::tools::convertToXML<l1menu::TriggerMenu>( const l1menu::TriggerMenu& object, l1menu::tools::XMLElement& parent )
{
	//return parent;
}
