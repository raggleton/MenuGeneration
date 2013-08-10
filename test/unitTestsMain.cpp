/** @file Copied this almost verbatim from the cppunit example cppunit/examples/simple
 *
 * Main function to run any registered test suites. Which test suites and the name
 * of the executable produced depends on whatever is setup in BuildFile.xml. At time
 * of writing the executable will be called "SimGeneral_TrackingAnalysis_unitTests"
 * and the test suites will be any in this directory that have a filename ending
 * in "_UnitTests.cpp".
 *
 * Remember that if you add any more *UnitTests.cpp files to the directory they won't
 * be noticed unless you update the modification time of BuildFile.xml, e.g. with
 * "touch BuildFile.xml".
 *
 * @author copied by Mark Grimes (mark.grimes@bristol.ac.uk)
 * @date 26/Jul/2013
 */

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include "TestParameters.h"
#include "l1menu/tools/CommandLineParser.h"


/** @brief Subclass the parameter store so that I can have methods to modify the parameters.
 *
 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
 * @date 12/Aug/2013
 */
template<class T>
class MutableTestParameters : public TestParameters<T>
{
public:
	static void setParameter( const std::string& parameterName, T parameterValue )
	{
		TestParameters<T>::parameters_[parameterName]=parameterValue;
	}
};

void printUsage( const std::string& executableName, std::ostream& output=std::cout )
{
	output << "Usage:" << "\n"
			<< "\t" << executableName << "\n"
			<< "\t" << "\t" << "runs the unit tests with the hard coded default input filenames." << "\n"
			<< "\n"
			<< "\t" << executableName << " [test input file] [test menu file]" << "\n"
			<< "\t" << "\t" << "runs the unit tests with input files named" << "\n"
			<< "\n"
			<< "\t" << executableName << " --help" << "\n"
			<< "\t" << "\t" << "prints this help message"
			<< "\n"
			<< std::endl;
}

/** @brief Parses the command line.
 *
 * @return                         Returns true if everything went okay, false if the program should
 *                                 exit without error e.g. if "help" option was specified the program
 *                                 should exit with no error.
 * @throw   std::runtime_error     If anything goes wrong and the program should exit with an error.
 */
bool handleCommandLine( int argc, char* argv[] )
{
	l1menu::tools::CommandLineParser commandLineParser;
	commandLineParser.addOption( "help", l1menu::tools::CommandLineParser::NoArgument );

	try{ commandLineParser.parse( argc, argv ); }
	catch( std::runtime_error& exception )
	{
		// I just want to print the usage here and let the caller handle the exception
		printUsage( commandLineParser.executableName(), std::cerr );
		throw;
	}

	if( commandLineParser.optionHasBeenSet( "help" ) )
	{
		printUsage( commandLineParser.executableName() );
		return false;
	}

	if( commandLineParser.nonOptionArguments().size()>2 )
	{
		printUsage( commandLineParser.executableName(), std::cerr );
		throw std::runtime_error( "Too many command line arguments" );
	}

	//
	// Need to set filenames for an input sample and a menu for the tests to run on. I'll
	// first see if the user specified them on the command line, if not I'll use hard
	// coded defaults.
	//
	if( commandLineParser.nonOptionArguments().size()>0 ) MutableTestParameters<std::string>::setParameter( "TEST_SAMPLE_FILENAME", commandLineParser.nonOptionArguments()[0] );
	else
	{
		std::string filename="";
		char* pEnvironmentVariable=std::getenv("HOME");
		if( pEnvironmentVariable!=nullptr ) filename=pEnvironmentVariable+std::string("/");
		filename+="MenuGenerationFiles/Fallback_NeutrinoGun_PU100.proto";
		std::cerr << "Input sample filename not specified on the command line, so using the default of " << filename << std::endl;
		MutableTestParameters<std::string>::setParameter( "TEST_SAMPLE_FILENAME", filename );
	}

	if( commandLineParser.nonOptionArguments().size()>1 ) MutableTestParameters<std::string>::setParameter( "TEST_MENU_FILENAME", commandLineParser.nonOptionArguments()[1] );
	else
	{
		std::string filename="";
		char* pEnvironmentVariable=std::getenv("CMSSW_BASE");
		if( pEnvironmentVariable!=nullptr ) filename=pEnvironmentVariable+std::string("/");
		filename+="src/UserCode/L1TriggerUpgrade/marksStuff/L1Menu_v17m20_std.txt";
		std::cerr << "Input menu filename not specified on the command line, so using the default of " << filename << std::endl;
		MutableTestParameters<std::string>::setParameter( "TEST_MENU_FILENAME", filename );
	}

	return true;
}

int main( int argc, char* argv[] )
{
	try
	{
		// Exit if something on the command line says so (e.g. "help" option should print usage and exit).
		if( !handleCommandLine( argc, argv ) ) return 0;
	}
	catch( std::runtime_error& exception )
	{
		std::cerr << "Error parsing the command line: " << exception.what() << std::endl;
		return -1;
	}


	// Create the event manager and test controller
	CPPUNIT_NS::TestResult controller;

	// Add a listener that colllects test result
	CPPUNIT_NS::TestResultCollector result;
	controller.addListener( &result );

	// Add a listener that print dots as test run.
	CPPUNIT_NS::BriefTestProgressListener progress;
	controller.addListener( &progress );

	// Add the top suite to the test runner
	CPPUNIT_NS::TestRunner runner;
	runner.addTest( CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest() );
	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write();

	return result.wasSuccessful() ? 0 : 1;
}
