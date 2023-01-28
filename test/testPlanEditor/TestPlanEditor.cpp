#include "TestPlanEditor.hpp"

#include "widgets/planedit/PlanEditor.hpp"
#include "uptime/ConnectionType.hpp"

#include "Utility_test.hpp"

void TestPlanEditor::test()
{
	PlanEditor w, *wp=&w;
	w.show();
	QTimer t;
	t.start(10);
	connect(&t,&QTimer::timeout, this, [=]() {
		
	}, OC_CONTYPE_NON_UNIQUE);

	test::utility::waitForUIEnd(wp);
}


OC_TEST_MAIN(test, TestPlanEditor)

