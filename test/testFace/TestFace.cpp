#include "TestFace.hpp"


#include "md/nodes/Graph.hpp"
#include "md/nodes/NodeView.hpp"
#include "test/Utility.hpp"
#include "uptime/New.hpp"


void TestFace::testFace()
{
	Q_INIT_RESOURCE(icons);
	auto graph = QSharedPointer<Graph>::create();
	auto nv = OC_NEW NodeView();
	nv->setGraph(graph);
	nv->show();
	test::utility::waitForUIEnd(nv);
}





OC_TEST_MAIN(test, TestFace)



