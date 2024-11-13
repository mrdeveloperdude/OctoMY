#include "TestDecimalEntry.hpp"

#include "components/DecimalEntry.hpp"
#include "uptime/New.hpp"
#include "test/Utility.hpp"

void TestDecimalEntry::test()
{
	Q_INIT_RESOURCE(icons);
	auto de = OC_NEW DecimalEntry();
	de->show();
	test::utility::waitForUIEnd(de);
}


OC_TEST_MAIN(test, TestDecimalEntry)

