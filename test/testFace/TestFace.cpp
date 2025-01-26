#include "TestFace.hpp"


#include "d3/nodes/Project.hpp"
#include "d3/nodes/NodeView.hpp"
#include "test/Utility.hpp"
#include "uptime/New.hpp"


void TestFace::testFace()
{
	Q_INIT_RESOURCE(icons);
	auto project = createProject();
	auto nv = OC_NEW NodeView();
	nv->setProject(project);
	nv->show();
	qDebug()<<"FACE";
	test::utility::waitForUIEnd(nv);
}





OC_TEST_MAIN(test, TestFace)



