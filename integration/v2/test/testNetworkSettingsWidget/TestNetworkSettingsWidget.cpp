#include "TestNetworkSettingsWidget.hpp"

#include "NetworkSettingsTestWidget.hpp"

#include "Utilities.hpp"

#include <QSignalSpy>
#include <QWidget>


void TestNetworkSettingsWidget::test()
{
	NetworkSettingsTestWidget *nstw=new NetworkSettingsTestWidget();
	nstw->setAttribute(Qt::WA_DeleteOnClose);
	nstw->show();
	QSignalSpy spy(nstw, &NetworkSettingsTestWidget::destroyed);
	spy.wait(1000000);
	//
	//waitForUiEnd(&tw);
}


OC_TEST_MAIN(test, TestNetworkSettingsWidget)
