#include "TestHardwareWidget.hpp"

#include "hardware/HardwareWizard.hpp"
#include "test/Utility.hpp"

#include "agent/Agent.hpp"
#include "app/launcher/AppLauncher.hpp"

void TestHardwareWidget::test()
{
	HardwareWizard *hw=new HardwareWizard(nullptr);
	QVERIFY(nullptr!=hw);
	QSharedPointer<AppLauncher<Agent> > agentMain=QSharedPointer<AppLauncher<Agent> >(OC_NEW AppLauncher<Agent>());
	QVERIFY(!agentMain.isNull());
	agentMain->run();
	hw->configure(agentMain->app());
	hw->show();
	test::utility::waitForUIEnd(hw);
	hw->deleteLater();
	hw=nullptr;
}


OC_TEST_MAIN(test, TestHardwareWidget)
