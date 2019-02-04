#include "NodeWindow.hpp"

#include "uptime/MethodGate.hpp"
#include "app/Settings.hpp"


#include "node/Node.hpp"

#include <QDebug>
#include <QRect>
#include <QApplication>
#include <QDesktopWidget>
#include <QVariant>

NodeWindow::NodeWindow(QWidget *parent)
	: QWidget(parent)
	, mNode(nullptr)
{
	OC_METHODGATE();
}


NodeWindow::~NodeWindow()
{
	OC_METHODGATE();
}



void NodeWindow::nodeWindowConfigure(QSharedPointer<Node> node)
{
	OC_METHODGATE();
	// Dissconnect old node
	if(!mNode.isNull()) {
// [...]


	}
	// Set new node
	qDebug()<<"SETTING NODE TO "<<node;
	mNode=node;
	// Connect new agent
	if(!mNode.isNull()) {

// [...]
		configure();
	} else {
		qWarning()<<"WARNING: No Agent in agent window";
	}


}


QSharedPointer<Node> NodeWindow::node()
{
	OC_METHODGATE();
	return mNode;
}


void NodeWindow::loadWindowGeometry()
{
	OC_METHODGATE();
#ifdef Q_OS_ANDROID
	showFullScreen();
#else
	QByteArray geometry;
	if(!mNode.isNull()) {
		QSharedPointer<Settings> s=mNode->settings();
		geometry = s->getCustomSettingByteArray("geometry", QByteArray());
	}
	//qDebug()<<"#*#*#*#*#*#*#*#* LOADED GEOMETRY WAS "<<geometry;
	if (geometry.isEmpty()) {
		qDebug()<<"WARNING: No window geometry found in settings";
		const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
		resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
		move((availableGeometry.width() - width()) / 2,
			 (availableGeometry.height() - height()) / 2);
	} else {
		//qDebug()<<"Window geometry found in settings, restoring.";
		restoreGeometry(geometry);
	}

#endif
}

void NodeWindow::saveWindowGeometry()
{
	OC_METHODGATE();
	//qDebug()<<"#*#*#*#*#*#*#*#* SAVING GEOMETRY REACHED IN NODEWIN";
	if(!mNode.isNull()) {
		QSharedPointer<Settings> s=mNode->settings();
		QByteArray geometry = saveGeometry();
		//qDebug()<<"#*#*#*#*#*#*#*#* SAVING GEOMETRY "<<geometry;
		s->setCustomSettingByteArray("geometry", geometry);
	}
}



///////////////////////////////////////////////////////////////////////////////

void NodeWindow::notifyAndroid(QString s)
{
	OC_METHODGATE();
	(void)s;
	//TODO: This crashes with some jni exception stuff. Figure out why
#ifdef Q_OS_ANDROID
	qDebug()<<"QT: Android NOTIFE: "<<s;
	QAndroidJniObject::callStaticMethod<void>("org/octomy/Agent/Agent",
			"notify",
			"(Ljava/lang/String;)V",
			QAndroidJniObject::fromString(s).object<jstring>());
#endif
}


void NodeWindow::toastAndroid(QString s)
{
	OC_METHODGATE();
	(void)s;
	//TODO: This crashes with some jni exception stuff. Figure out why
#ifdef Q_OS_ANDROID
	qDebug()<<"QT: Android TOAST: "<<s;
	QAndroidJniObject::callStaticMethod<void>("org/octomy/Agent/Agent",
			"toast",
			"(Ljava/lang/String;)V",
			QAndroidJniObject::fromString(s).object<jstring>());
#endif
}
