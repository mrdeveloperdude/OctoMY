#include "TestActuatorWidget.hpp"

#include "hardware/actuator/ActuatorWidget.hpp"
#include "uptime/New.hpp"
#include "test/Utility.hpp"


void TestActuatorWidget::test()
{
	Q_INIT_RESOURCE(icons);
	auto aw = OC_NEW ActuatorWidget();
	aw->show();
	test::utility::waitForUIEnd(aw);
}


OC_TEST_MAIN(test, TestActuatorWidget)

