#include "TestPoseMappingWidget.hpp"

#include "ui_TestWindow.h"

#include "../../libs/libcore/puppet/PoseMapping.hpp"

#include <QPixmap>
#include <QImage>

TestWindow::TestWindow(QWidget *parent, QString name) :
	QWidget(parent),
	ui(new Ui::TestWindow)
{
	ui->setupUi(this);
	setWindowTitle(name);
}

TestWindow::~TestWindow()
{
	delete ui;
}

void TestWindow::on_pushButtonClose_clicked()
{
	close();
}

void TestWindow::configure(PoseMapping &pm)
{
	ui->widgetPoseMapping->configure(pm);
}



// YOU NEED THIS: http://doc.qt.io/qt-5/qtest.html
void TestPoseMappingWidget::test()
{
	TestWindow tw(nullptr, "Pose Mapping Widget Test");
	tw.show();
	qApp->processEvents();
	QTest::qWait(1000);
	PoseMapping pm(3);
	pm.setName(0, "Throttle");
	pm.setName(1, "Steering");
	pm.setName(2, "CameraAngle");
	tw.configure(pm);
	qApp->processEvents();
	QTest::qWait(100000);
}



QTEST_MAIN(TestPoseMappingWidget)
