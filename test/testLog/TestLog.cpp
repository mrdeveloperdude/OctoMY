#include "TestLog.hpp"

#include "test/Utility.hpp"
#include "TestLogWidget.hpp"

#include <QApplication>
#include <QDebug>
#include <QTimer>
#include <QWidget>




void TestLog::test()
{
	qDebug() << "Testing the test framework";
	TestLogWidget w;
	w.setWindowTitle("LogWidget Test");
	w.show();
	test::utility::waitForUIEnd(&w);
}


OC_TEST_MAIN(test, TestLog)

