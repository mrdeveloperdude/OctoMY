#include "AgentWindow.hpp"
#include "ui_AgentWindow.h"


#include "agent/Agent.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "uptime/ConnectionType.hpp"

#include "app/Settings.hpp"


#include <QDebug>
#include <QMessageBox>
#include <QMap>

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#endif


#include "pose/NameMapping.hpp"
#include "widgets/NameMappingWidget.hpp"


AgentWindow::AgentWindow(QWidget *parent)
	: NodeWindow(parent)
	, ui(OC_NEW Ui::AgentWindow)
{
	OC_METHODGATE();
	ui->setupUi(this);
	loadWindowGeometry();
}

AgentWindow::~AgentWindow()
{
	OC_METHODGATE();
	saveWindowGeometry();
	delete ui;
	ui=nullptr;
}


QSharedPointer<Agent> AgentWindow::agent()
{
	OC_METHODGATE();
	QSharedPointer<Node> n=node();
	if(n.isNull()) {
		qWarning()<<"WARNING: No node in agent()";
	}

	return qSharedPointerCast<Agent> (n);
}


void AgentWindow::configure()
{
	OC_METHODGATE();
	auto a=agent();
	if(!a.isNull()) {
// [...]
	} else {
		qWarning()<<"WARNING: No Agent in agent window configure";
	}
}



void AgentWindow::appendLog(const QString& text)
{
	OC_METHODGATE();
	qDebug()<<"AGENT-LOG-APPEND: "<<text;
	//ui->widgetFace->appendLog(text);
}




void AgentWindow::keyReleaseEvent(QKeyEvent *e)
{
	OC_METHODGATE();
	if(Qt::Key_Back==e->key()) {
		/*
		if(ui->pageRunning==ui->stackedWidget->currentWidget()) {
			appendLog("GOING TO CONFIRM QUIT SCREEN ON BACK BUTTON");
			onStartQuitApplication();
		} else if(ui->pageConfirmQuit==ui->stackedWidget->currentWidget()) {
			appendLog("GOING TO RUNNING SCREEN ON BACK BUTTON");
			setCurrentPage(ui->pageRunning);
		} else {
			appendLog("ERROR ON BACK BUTTON");
		}
		*/
		e->accept();
	} else {
		//appendLog("UNKNOWN BUTTON: "+QString::number(e->key()));
	}
}


void AgentWindow::closeEvent(QCloseEvent *event)
{
	OC_METHODGATE();
	//qDebug()<<"closeEvent()";
	Q_UNUSED(event);
	//shutDown();
}



void AgentWindow::on_pushButtonTestQuit_clicked()
{
	OC_METHODGATE();
	// qDebug()<<"close button clicked ()";
	nodeWindowRequestExit(EXIT_SUCCESS);
}
