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

void TestPoseMappingWidget::testBasics()
{

	PoseMapping pm(5);
	pm.setName(0,"Arne");
	pm.setName(1,"Bjarne");
	pm.setName(2,"Peder");
	pm.setName(3,"Kjell");
	pm.setName(4,"Nils");

	pm.setMapping(0,1);
	pm.setMapping(1,3);
	pm.setMapping(2,4);
	pm.setMapping(3,0);
	pm.setMapping(4,2);

	qDebug()<<"PM1 BEFORE:"<<pm.toString();
	QVariantList map=pm.toMap();
	qDebug()<<"PM1 AFTER: "<<pm.toString();

	PoseMapping pm2(0);

	qDebug()<<"PM2 BEFORE:"<<pm2.toString();
	pm2.fromMap(map);

	qDebug()<<"PM2 AFTER: "<<pm2.toString();
}

// YOU NEED THIS: http://doc.qt.io/qt-5/qtest.html
void TestPoseMappingWidget::testWidget()
{
	return;//Skip for now

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
