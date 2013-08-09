#ifndef l1menu_tools_stringManipulation_h
#define l1menu_tools_stringManipulation_h

/** @file Miscellaneous useful functions related to string manipulation.
 */

#include <vector>
#include <string>

namespace l1menu
{
	namespace tools
	{
		/** @brief Converts the entire string to a float or throws an exception.
		 *
		 * @param[in] string    The string to convert.
		 * @return              The float that the string represents, if every character could
		 *                      be converted. If there are any problems a std::runtime_error is
		 *                      thrown.
		 *
		 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
		 * @date 28/May/2013
		 */
		float convertStringToFloat( const std::string& string );

		/** Splits a string into individual parts delimited by whitespace.
		 *
		 * Whitespace is defined as any of "\x20\x09\x0D\x0A", i.e. space, tab, carriage return
		 * of line feed.
		 *
		 * @param[in] stringToSplit    The string to split into elements delimited by whitespace.
		 * @return                     A vector where each element is a portion of the string.

		 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
		 * @date 28/May/2013
		 */
		std::vector<std::string> splitByWhitespace( const std::string& stringToSplit );

		/** Splits a string into individual parts delimited by a user defined delimeter(s).
		 *
		 * The user can specify more than one character in the delimeter, in which case any of
		 * them will be used, rather than the whole string.
		 *
		 * Note that this functions differently to splitByWhitespace in that any single delimeter
		 * will produce a new element - multiple contiguous delimeters will produce multiple empty
		 * elements in the return value. In splitByWhitespace contiguous whitespace is considered
		 * as a single delimeter.
		 *
		 * @param[in] stringToSplit    The string to split into elements.
		 * @param[in] delimeters       The delimeters to use. If more than one character is specified
		 *                             then any single one is considered a delimeter.
		 * @return                     A vector where each element is a portion of the string.

		 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
		 * @date 16/Jul/2013
		 */
		std::vector<std::string> splitByDelimeters( const std::string& stringToSplit, const std::string& delimeters );

	} // end of the tools namespace
} // end of the l1menu namespace
#endif
