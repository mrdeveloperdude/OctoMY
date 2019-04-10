#include "RemoteWindow.hpp"
#include "ui_RemoteWindow.h"

#include "remote/Remote.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "uptime/ConnectionType.hpp"

#include <QDebug>


#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#endif

RemoteWindow::RemoteWindow(QWidget *parent)
	: NodeWindow(parent)
	, ui(OC_NEW Ui::RemoteWindow)
{
	OC_METHODGATE();
	ui->setupUi(this);
}

RemoteWindow::~RemoteWindow()
{
	OC_METHODGATE();
	saveWindowGeometry();
	delete ui;
	ui=nullptr;
}



QSharedPointer<Remote> RemoteWindow::remote()
{
	OC_METHODGATE();
	QSharedPointer<Node> n=node();
	if(n.isNull()) {
		qWarning()<<"WARNING: No node in remote()";
	}

	return qSharedPointerCast<Remote> (n);
}




void RemoteWindow::configure()
{
	OC_METHODGATE();
	auto r=remote();
	if(!r.isNull()) {
// [...]
		loadWindowGeometry();
		ui->widgetDebugButton->configure(node(), true);
	} else {
		qWarning()<<"WARNING: No Remote in remote window configure";
	}
}




void RemoteWindow::appendLog(const QString& text)
{
	OC_METHODGATE();
	qDebug()<<"REMOTE-LOG-APPEND: "<<text;
}




