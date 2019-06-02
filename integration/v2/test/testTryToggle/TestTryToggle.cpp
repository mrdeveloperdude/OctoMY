#include "TestTryToggle.hpp"

#include "widgets/TryToggle.hpp"

#include "TryToggleTestUI.hpp"

#include "Utility_test.hpp"

#include <QWidget>
#include <QLayout>
#include <QGridLayout>
#include <QSignalSpy>


void TestTryToggle::test()
{
	TryToggle *tt=new TryToggle();
	QWidget *w=new QWidget();
	tt->configure("OFF", "GOING_ON", "ON", "GOING_OFF", QColor("green"), QColor("white"));
	w->setLayout(new QGridLayout());
	auto l=w->layout();
	l->addWidget(tt);
	w->show();
	test::utility::waitForUIEnd(w);
}


void TestTryToggle::testUI()
{
	TryToggleTestUI ttui;
	ttui.show();
	test::utility::waitForUIEnd(&ttui);
}


OC_TEST_MAIN(test, TestTryToggle)
