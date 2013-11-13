#ifndef l1menu_tools_XMLElement_h
#define l1menu_tools_XMLElement_h

#include <string>
#include <memory>
#include <iosfwd>
#include <vector>

// Can't use a forward declaration because of the odd way xerces declares the namespace
#include <xercesc/dom/DOMElement.hpp>

namespace l1menu
{
	namespace tools
	{
		/** @brief Class to mediate between the calling code and the underlying XML parser library.
		 *
		 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
		 * @date 08/Oct/2013
		 */
		class XMLElement
		{
			friend class XMLFile;
		public:
			XMLElement( const l1menu::tools::XMLElement& otherElement );
			XMLElement( l1menu::tools::XMLElement&& otherElement ) noexcept;
			XMLElement& operator=( const l1menu::tools::XMLElement& otherElement );
			XMLElement& operator=( l1menu::tools::XMLElement&& otherElement ) noexcept;
			~XMLElement();

			std::string name() const;
			std::vector<l1menu::tools::XMLElement> getChildren();
			const std::vector<l1menu::tools::XMLElement> getChildren() const;
			std::vector<l1menu::tools::XMLElement> getChildren( const std::string& name );
			const std::vector<l1menu::tools::XMLElement> getChildren( const std::string& name ) const;
			l1menu::tools::XMLElement createChild( const std::string& name );
			bool hasAttribute( const std::string& name ) const;
			std::string getAttribute( const std::string& name ) const;
			int getIntAttribute( const std::string& name ) const;
			float getFloatAttribute( const std::string& name ) const;
			void setAttribute( const std::string& name, const std::string& value );
			void setAttribute( const std::string& name, int value );
			void setAttribute( const std::string& name, float value );
			std::string getValue() const;
			int getIntValue() const;
			float getFloatValue() const;
			void setValue( const std::string& value );
			void setValue( int value );
			void setValue( float value );
		private:
			XMLElement( xercesc::DOMElement* pRawElement, xercesc::DOMDocument* pDocument );
			xercesc::DOMElement* pRawElement_;
			xercesc::DOMDocument* pDocument_;
			//std::unique_ptr<ElementPrivateMembers> pImple;
		};

	} // end of namespace tools
} // end of namespace l1menu

#endif
