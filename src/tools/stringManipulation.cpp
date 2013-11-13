#include "l1menu/tools/stringManipulation.h"

#include <sstream>
#include <stdexcept>

// use the unnamed namespace for things only used in this file
namespace
{
	/// ASCII codes of characters that are considered whitespace (space, tab, carriage return, line feed).
    const char* whitespace="\x20\x09\x0D\x0A";
} // end of the unnamed namespace

float l1menu::tools::convertStringToFloat( const std::string& string )
{
	float returnValue;
	std::stringstream stringConverter( string );
	stringConverter >> returnValue;
	if( stringConverter.fail() || !stringConverter.eof() ) throw std::runtime_error( "Unable to convert \""+string+"\" to a float" );
	return returnValue;
}

int l1menu::tools::convertStringToInt( const std::string& string )
{
	int returnValue;
	std::stringstream stringConverter( string );
	stringConverter >> returnValue;
	if( stringConverter.fail() || !stringConverter.eof() ) throw std::runtime_error( "Unable to convert \""+string+"\" to an int" );
	return returnValue;
}

std::vector<std::string> l1menu::tools::splitByWhitespace( const std::string& stringToSplit )
{
	std::vector<std::string> returnValue;

	size_t currentPosition=0;
	size_t nextDelimeter=0;
	do
	{
		// Skip over any leading whitespace
		size_t nextElementStart=stringToSplit.find_first_not_of( ::whitespace, currentPosition );
		if( nextElementStart!=std::string::npos ) currentPosition=nextElementStart;

		// Find the next whitespace and subtract everything up to that point
		nextDelimeter=stringToSplit.find_first_of( ::whitespace, currentPosition );
		std::string element=stringToSplit.substr( currentPosition, nextDelimeter-currentPosition );
		returnValue.push_back(element);

		// skip over any trailing whitespace
		nextElementStart=stringToSplit.find_first_not_of( ::whitespace, nextDelimeter );
		if( nextElementStart!=std::string::npos ) currentPosition=nextElementStart;
		else nextDelimeter=std::string::npos;

	} while( nextDelimeter!=std::string::npos );

	return returnValue;
}

std::vector<std::string> l1menu::tools::splitByDelimeters( const std::string& stringToSplit, const std::string& delimeters )
{
	std::vector<std::string> returnValue;

	size_t currentPosition=0;
	size_t nextDelimeter=0;
	do
	{
		// Find the next occurence of one of the delimeters and subtract everything up to that point
		nextDelimeter=stringToSplit.find_first_of( delimeters, currentPosition );
		std::string element=stringToSplit.substr( currentPosition, nextDelimeter-currentPosition );
		returnValue.push_back(element);

		// Move on to the next part of the string
		if( nextDelimeter+1<stringToSplit.size() ) currentPosition=nextDelimeter+1;
		else nextDelimeter=std::string::npos;

	} while( nextDelimeter!=std::string::npos );

	return returnValue;
}
