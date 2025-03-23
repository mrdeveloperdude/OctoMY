#include "TestConstruct.hpp"


#include "construct/ConstructShell.hpp"
#include "test/Utility.hpp"
#include "uptime/New.hpp"

void TestConstruct::test()
{
	Q_INIT_RESOURCE(icons);
	auto w = OC_NEW ConstructShell();
	w->show();
	test::utility::waitForUIEnd(w);
}



OC_TEST_MAIN(test, TestConstruct)

