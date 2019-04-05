#include "AgentWindow.hpp"
#include "ui_AgentWindow.h"


#include "agent/Agent.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "uptime/ConnectionType.hpp"

#include "app/Settings.hpp"


#include <QDebug>

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
		loadWindowGeometry();
		ui->widgetDebugger->configure(node());
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

