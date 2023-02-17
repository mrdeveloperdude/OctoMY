#include "MockAppWindow.hpp"
#include "ui_MockAppWindow.h"

#include "app/mockapp/MockApp.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"



MockAppWindow::MockAppWindow(QWidget *parent)
	: NodeWindow(parent)
	, ui(OC_NEW Ui::MockAppWindow)
{
	OC_METHODGATE();
	ui->setupUi(this);
}

MockAppWindow::~MockAppWindow()
{
	OC_METHODGATE();
	saveWindowGeometry();
	delete ui;
	ui=nullptr;
}



QSharedPointer<MockApp> MockAppWindow::mockApp()
{
	OC_METHODGATE();
	QSharedPointer<Node> n=node();
	if(n.isNull()) {
		qWarning()<<"WARNING: No node in mockApp()";
	}
	return qSharedPointerCast<MockApp> (n);
}


void MockAppWindow::configure()
{
	OC_METHODGATE();
	auto a=mockApp();
	if(!a.isNull()) {
// [...]
		loadWindowGeometry();
		//ui->widgetDebuggerButton->configure(node(), true);
		updateWindowIcon();
		//ui->widgetPairing->configure(node());
		//ui->wi
		
		QObject::connect(
		ui->pushButton, &QPushButton::clicked, this, [=]() {
			qDebug()<<"Quit clicked";
			emit a->nodeRequestExit(EXIT_SUCCESS);
		});
		
		
	} else {
		qWarning()<<"WARNING: No Agent in agent window configure";
	}
}



void MockAppWindow::appendLog(const QString& text)
{
	OC_METHODGATE();
	qDebug()<<"MOCKAP-LOG-APPEND: "<<text;
	//ui->widgetFace->appendLog(text);
}
