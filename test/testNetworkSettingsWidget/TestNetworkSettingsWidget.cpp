#include "TestNetworkSettingsWidget.hpp"

#include "widgets/NetworkSettingsWidget.hpp"

#include "basic/LocalAddressList.hpp"


#include <QSignalSpy>
#include <QWidget>


void TestNetworkSettingsWidget::test()
{

	Q_INIT_RESOURCE(icons);
	NetworkSettingsWidget *nsw=new NetworkSettingsWidget();


	nsw->setAttribute(Qt::WA_DeleteOnClose);

	//LocalAddressList list(1337);	nsw->configure(list);


	nsw->show();

	QSignalSpy spy(nsw, &NetworkSettingsWidget::destroyed);
	spy.wait(1000000);
}



QTEST_MAIN(TestNetworkSettingsWidget)
