#include "TestNodesManagementWidget.hpp"

#include "hub/NodesManagementWidget.hpp"

#include <QSignalSpy>
#include <QWidget>


void TestNodesManagementWidget::test(){
	auto nmw = new NodesManagementWidget();
	nmw->setAttribute(Qt::WA_DeleteOnClose);
	nmw->show();
	QSignalSpy spy(nmw, &NodesManagementWidget::destroyed);
	spy.wait(1000000);
}

OC_TEST_MAIN(test, TestNodesManagementWidget)
