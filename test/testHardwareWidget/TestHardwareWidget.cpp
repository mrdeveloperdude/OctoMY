#include "TestHardwareWidget.hpp"

#include "widgets/HardwareWizard.hpp"

#include "agent/Agent.hpp"
#include "app/launcher/AppLauncher.hpp"

void TestHardwareWidget::test()
{
	HardwareWizard *hw=new HardwareWizard(nullptr);
	QVERIFY(nullptr!=hw);
	NodeLauncher<Agent> agentLauncher;
	agentLauncher.start();
	hw->configure(agentLauncher.node());
	hw->show();
	waitForUIEnd(hw);
	hw->deleteLater();
	hw=nullptr;
}


OC_TEST_MAIN(test, TestHardwareWidget)
