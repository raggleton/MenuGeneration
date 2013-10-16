#include "l1menu/tools/XMLElement.h"

#include <stdexcept>
#include <sstream>
#include <iostream>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include "l1menu/tools/stringManipulation.h"

namespace // Use the unnamed namespace for things only used in this file
{
	/** @brief Class copied from a xerces example to convert a std::string into the format xerces uses.
	 * @author Mark Grimes (mark.grimes@bristol.ac.uk) but copied from CreateDOMDocument.cpp in the xerces samples
	 * @date 02/Oct/2013
	 */
	class XercesString
	{
	public:
		XercesString( const char* cString );
		XercesString( const std::string& string );
		~XercesString();
		const XMLCh* get();
	protected:
		XMLCh*   pXMLString_;
	};

	/** @brief Corollary of the XercesString class to convert xerces format strings to native C strings.
	 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
	 * @date 03/Oct/2013
	 */
	class NativeString
	{
	public:
		NativeString( const XMLCh* xercesString );
		~NativeString();
		const char* get();
	protected:
		char*   pNativeString_;
	};

	XercesString::XercesString( const char* cString ) { pXMLString_=xercesc::XMLString::transcode(cString); }
	XercesString::XercesString( const std::string& string ) { pXMLString_=xercesc::XMLString::transcode(string.c_str()); }
	XercesString::~XercesString() { xercesc::XMLString::release(&pXMLString_); }
	const XMLCh* XercesString::get() { return pXMLString_; }

	NativeString::NativeString( const XMLCh* xercesString ) { pNativeString_=xercesc::XMLString::transcode(xercesString); }
	NativeString::~NativeString() { xercesc::XMLString::release(&pNativeString_); }
	const char* NativeString::get() { return pNativeString_; }

} // end of the unnamed namespace

l1menu::tools::XMLElement::XMLElement( xercesc::DOMElement* pRawElement, xercesc::DOMDocument* pDocument )
	: pRawElement_(pRawElement), pDocument_(pDocument)
{
	// No operation besides the initialiser list
}

l1menu::tools::XMLElement::XMLElement( const l1menu::tools::XMLElement& otherElement )
	: pRawElement_(otherElement.pRawElement_), pDocument_(otherElement.pDocument_)
{
	// No operation besides the initialiser list
}

l1menu::tools::XMLElement::XMLElement( l1menu::tools::XMLElement&& otherElement ) noexcept
	: pRawElement_(otherElement.pRawElement_), pDocument_(otherElement.pDocument_)
{
	// No operation besides the initialiser list
}

l1menu::tools::XMLElement& l1menu::tools::XMLElement::operator=( const l1menu::tools::XMLElement& otherElement )
{
	pRawElement_=otherElement.pRawElement_;
	pDocument_=otherElement.pDocument_;
	return *this;
}

l1menu::tools::XMLElement& l1menu::tools::XMLElement::operator=( l1menu::tools::XMLElement&& otherElement ) noexcept
{
	pRawElement_=otherElement.pRawElement_;
	pDocument_=otherElement.pDocument_;
	return *this;
}

l1menu::tools::XMLElement::~XMLElement()
{
	// No operation
}

std::string l1menu::tools::XMLElement::name() const
{
	return ::NativeString( pRawElement_->getTagName() ).get();
}

std::vector<l1menu::tools::XMLElement> l1menu::tools::XMLElement::getChildren()
{
	std::vector<XMLElement> returnValue;

	xercesc::DOMNodeList* pChildNodes=pRawElement_->getChildNodes();
	for( size_t index=0; index<pChildNodes->getLength(); ++index )
	{
		xercesc::DOMNode* pChild=pChildNodes->item(index);
		xercesc::DOMElement* pChildElement=dynamic_cast<xercesc::DOMElement*>( pChild );
		if( pChildElement!=nullptr ) returnValue.push_back( XMLElement(pChildElement,pDocument_) );
	}

	return returnValue;
}

const std::vector<l1menu::tools::XMLElement> l1menu::tools::XMLElement::getChildren() const
{
	std::vector<XMLElement> returnValue;

	xercesc::DOMNodeList* pChildNodes=pRawElement_->getChildNodes();
	for( size_t index=0; index<pChildNodes->getLength(); ++index )
	{
		xercesc::DOMNode* pChild=pChildNodes->item(index);
		xercesc::DOMElement* pChildElement=dynamic_cast<xercesc::DOMElement*>( pChild );
		if( pChildElement!=nullptr ) returnValue.push_back( XMLElement(pChildElement,pDocument_) );
	}

	return returnValue;
}

std::vector<l1menu::tools::XMLElement> l1menu::tools::XMLElement::getChildren( const std::string& name )
{
	std::vector<XMLElement> returnValue;

	xercesc::DOMNodeList* pChildNodes=pRawElement_->getChildNodes();
	for( size_t index=0; index<pChildNodes->getLength(); ++index )
	{
		xercesc::DOMNode* pChild=pChildNodes->item(index);
		xercesc::DOMElement* pChildElement=dynamic_cast<xercesc::DOMElement*>( pChild );
		if( pChildElement!=nullptr )
		{
			if( ::NativeString( pChildElement->getTagName() ).get()==name ) returnValue.push_back( XMLElement(pChildElement,pDocument_) );
		}
	}

	return returnValue;
}

const std::vector<l1menu::tools::XMLElement> l1menu::tools::XMLElement::getChildren( const std::string& name ) const
{
	std::vector<XMLElement> returnValue;

	xercesc::DOMNodeList* pChildNodes=pRawElement_->getChildNodes();
	for( size_t index=0; index<pChildNodes->getLength(); ++index )
	{
		xercesc::DOMNode* pChild=pChildNodes->item(index);
		xercesc::DOMElement* pChildElement=dynamic_cast<xercesc::DOMElement*>( pChild );
		if( pChildElement!=nullptr )
		{
			if( ::NativeString( pChildElement->getTagName() ).get()==name ) returnValue.push_back( XMLElement(pChildElement,pDocument_) );
		}
	}

	return returnValue;
}

l1menu::tools::XMLElement l1menu::tools::XMLElement::createChild( const std::string& name )
{
	xercesc::DOMElement* pChildElement=pDocument_->createElement( XercesString(name).get() );
	pRawElement_->appendChild( pChildElement );
	return XMLElement(pChildElement,pDocument_);
}

bool l1menu::tools::XMLElement::hasAttribute( const std::string& name ) const
{
	return pRawElement_->hasAttribute( XercesString(name).get() );
}

std::string l1menu::tools::XMLElement::getAttribute( const std::string& name ) const
{
	const XMLCh* pAttribute=pRawElement_->getAttribute( XercesString(name).get() );
	if( pAttribute==nullptr ) throw std::runtime_error( "XMLElement has no attribute named "+name );
	return ::NativeString( pAttribute ).get();
}

int l1menu::tools::XMLElement::getIntAttribute( const std::string& name ) const
{
	return l1menu::tools::convertStringToInt( getAttribute(name) );
}

float l1menu::tools::XMLElement::getFloatAttribute( const std::string& name ) const
{
	return l1menu::tools::convertStringToFloat( getAttribute(name) );
}

void l1menu::tools::XMLElement::setAttribute( const std::string& name, const std::string& value )
{
	pRawElement_->setAttribute( XercesString(name).get(), XercesString(value).get() );
}

void l1menu::tools::XMLElement::setAttribute( const std::string& name, int value )
{
	std::stringstream stringConverter;
	stringConverter << value;
	pRawElement_->setAttribute( XercesString(name).get(), XercesString(stringConverter.str()).get() );
}

void l1menu::tools::XMLElement::setAttribute( const std::string& name, float value )
{
	std::stringstream stringConverter;
	stringConverter << value;
	pRawElement_->setAttribute( XercesString(name).get(), XercesString(stringConverter.str()).get() );
}

std::string l1menu::tools::XMLElement::getValue() const
{
	xercesc::DOMNodeList* pChildNodes=pRawElement_->getChildNodes();
	for( size_t index=0; index<pChildNodes->getLength(); ++index )
	{
		xercesc::DOMNode* pChild=pChildNodes->item(index);
		if( pChild->getNodeType()==xercesc::DOMNode::TEXT_NODE )
		{
			xercesc::DOMText* pValueNode=dynamic_cast<xercesc::DOMText*>(pChild);
			if( pValueNode!=nullptr )
			{
				try { return ::NativeString( pValueNode->getData() ).get(); }
				catch( xercesc::DOMException& error ) { std::cout << "error=" << ::NativeString( error.getMessage() ).get() << std::endl; }
			}
		}
		std::cout << "Child DOMNode name \"" << ::NativeString( pChild->getNodeName() ).get() << "\" type=" << pChild->getNodeType() << std::endl;
	}

	// If control got to here then no text child nodes were found and the element doesn't have a value.
	throw std::runtime_error( "XMLElement "+name()+" has no value set." );
}

int l1menu::tools::XMLElement::getIntValue() const
{
	return l1menu::tools::convertStringToInt( getValue() );
}

float l1menu::tools::XMLElement::getFloatValue() const
{
	return l1menu::tools::convertStringToFloat( getValue() );
}

void l1menu::tools::XMLElement::setValue( const std::string& value )
{
	// TODO - write something to clear any children in case there was something here before
	xercesc::DOMText* pValueNode=pDocument_->createTextNode( XercesString(value).get() );
	pRawElement_->appendChild( pValueNode );
}

void l1menu::tools::XMLElement::setValue( int value )
{
	std::stringstream stringConverter;
	stringConverter << value;
	setValue( stringConverter.str() );
}

void l1menu::tools::XMLElement::setValue( float value )
{
	// TODO - write something to make sure the precision is good enough
	std::stringstream stringConverter;
	stringConverter << value;
	setValue( stringConverter.str() );
}
