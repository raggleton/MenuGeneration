#ifndef l1menu_tools_conversionToXML_h
#define l1menu_tools_conversionToXML_h

/** @file
 * Functions to convert objects to XML
 */

#include "l1menu/tools/XMLElement.h"

//
// Forward declarations
//
namespace l1menu
{
	class TriggerMenu;
}
//#include "l1menu/TriggerMenu.h"

namespace l1menu
{
	namespace tools
	{
		template<class T> void convertToXML( const T& object, l1menu::tools::XMLElement& parent );
	} // end of the tools namespace
} // end of the l1menu namespace

// Use an #ifndef so that anybody using this file gets the "extern" definition.
// When I actually instantiate in the conversionToXML.cpp file I'll #define this before including
// this header, and then do the full instantiation. This way there will only be
// one instantiation.
#ifndef WILL_INSTANTIATE_l1menu_tools_conversionToXML_h

namespace l1menu
{
	namespace tools
	{
		extern template<> void convertToXML<l1menu::TriggerMenu>( const l1menu::TriggerMenu& object, l1menu::tools::XMLElement& parent );
		//extern template<> void convertToXML<l1menu::TriggerMenu>( const l1menu::TriggerMenu& object, l1menu::tools::XMLElement& parent );
	}
}

#endif // end of "#ifndef l1menu_tools_conversionToXML_h_WILL_INSTANTIATE"




#endif // end of "#ifndef l1menu_tools_conversionToXML_h"
