#include "TestCarSteeringWidget.hpp"

#include "ui_TestWindow.h"

#include "utility/Standard.hpp"

#include "Utilities.hpp"

#include <QPixmap>
#include <QImage>

TestWindow::TestWindow(QWidget *parent, QString name) :
	QWidget(parent),
	ui(OC_NEW Ui::TestWindow)
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


void TestCarSteeringWidget::test(){
	TestWindow tw(nullptr, "Car Steering Widget Test");
	tw.show();
	waitForUIEnd(&tw);
}



OC_TEST_MAIN(test, TestCarSteeringWidget)
