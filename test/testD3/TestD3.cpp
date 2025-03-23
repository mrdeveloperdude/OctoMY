#include "TestD3.hpp"

#include "d3/nodes/Project.hpp"
#include "d3/nodes/NodeView.hpp"
#include "d3/nodes/ui/SpreadSheet.hpp"
#include "test/Utility.hpp"
#include "uptime/New.hpp"

void TestD3::testNodes()
{
	Q_INIT_RESOURCE(icons);
	auto project = createProject();
	auto nv = OC_NEW NodeView();
	nv->setProject(project);	nv->show();
	qDebug()<<"D3 nodes";
	test::utility::waitForUIEnd(nv);
}

void TestD3::testSpreadsheet()
{
	Q_INIT_RESOURCE(icons);
	auto spreadSheet = OC_NEW SpreadSheet();	spreadSheet->show();
	qDebug()<<"D3 Spreadsheet";
	test::utility::waitForUIEnd(spreadSheet);
}


OC_TEST_MAIN(test, TestD3)

