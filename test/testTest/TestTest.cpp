#include "TestTest.hpp"

#include "test/Utility.hpp"
#include "uptime/ConnectionType.hpp"

#include <QWidget>


void TestTest::test()
{
	qDebug()<< "Testing the test framework";
	
	QWidget w, *wp=&w;
	w.setWindowTitle("TEST");
	w.show();
	test::utility::waitForUIEnd(wp);	
}


OC_TEST_MAIN(test, TestTest)


