#include "TestLightWidget.hpp"

#include "LightTestWidget.hpp"

#include <QSignalSpy>

void TestLightWidget::test()
{
	LightTestWidget *ltw=new LightTestWidget();
	ltw->setAttribute(Qt::WA_DeleteOnClose);
	ltw->show();
	QSignalSpy spy(ltw, &LightTestWidget::destroyed);
	spy.wait(1000000);
}


OC_TEST_MAIN(test, TestLightWidget)
