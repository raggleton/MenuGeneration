#include <cppunit/extensions/HelperMacros.h>


/** @brief A cppunit TestFixture to test getting triggers from the table and getting and
 * setting their parameters.
 *
 * @author Mark Grimes (mark.grimes@bristol.ac.uk)
 * @date 04/Aug/2013
 */
class TriggerTableUnitTestSuite : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE(TriggerTableUnitTestSuite);
	CPPUNIT_TEST(testGettingAndSettingAllTriggerParameters);
	CPPUNIT_TEST_SUITE_END();

protected:
	std::ostream* pVerboseOutput_;
public:
	void setUp();

protected:
	void testGettingAndSettingAllTriggerParameters();
};





#include <cppunit/config/SourcePrefix.h>
#include "l1menu/TriggerTable.h"
#include "l1menu/ITrigger.h"
#include <stdexcept>
#include <cmath>

CPPUNIT_TEST_SUITE_REGISTRATION(TriggerTableUnitTestSuite);

void TriggerTableUnitTestSuite::setUp()
{
	pVerboseOutput_=nullptr;
	//pVerboseOutput_=&std::cout;
}

void TriggerTableUnitTestSuite::testGettingAndSettingAllTriggerParameters()
{
	// Add a newline, because cppunit starts this function with half a line already written
	if( pVerboseOutput_!=nullptr ) *pVerboseOutput_ << "\n";
	l1menu::TriggerTable& table=l1menu::TriggerTable::instance();

	const auto& allTriggerDetails=table.listTriggers();

	// Loop over all of the triggers
	for( const auto& triggerDetails : allTriggerDetails )
	{
		std::unique_ptr<l1menu::ITrigger> pTrigger=table.getTrigger( triggerDetails.name, triggerDetails.version );
		CPPUNIT_ASSERT( pTrigger!=nullptr );

		if( pVerboseOutput_!=nullptr ) *pVerboseOutput_ << "Testing trigger " << triggerDetails.name << " v" << triggerDetails.version << std::endl;

		const auto& parameterNames=pTrigger->parameterNames();
		for( const auto& parameterName : parameterNames )
		{
			if( pVerboseOutput_!=nullptr ) *pVerboseOutput_ << "\t" << "testing parameter \"" << parameterName << "\"" << std::endl;

			CPPUNIT_ASSERT_NO_THROW( pTrigger->parameter(parameterName) );
			float newValue=std::rand();
			pTrigger->parameter(parameterName)=newValue;
			CPPUNIT_ASSERT_DOUBLES_EQUAL( newValue, pTrigger->parameter(parameterName), std::pow(10,-7) );
		}
	}
}
