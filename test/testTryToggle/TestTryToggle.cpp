#include "TestTryToggle.hpp"

#include "widgets/TryToggle.hpp"

#include "TryToggleTestUI.hpp"

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
	QSignalSpy spy(w, SIGNAL(close()));

	while(0==spy.count()) {
		spy.wait(100);
	}
}


void TestTryToggle::testUI()
{
	TryToggleTestUI ttui;
	ttui.show();
	QSignalSpy spy(&ttui, SIGNAL(close()));

	while(0==spy.count()) {
		spy.wait(100);
	}
}

QTEST_MAIN(TestTryToggle)
