#include "NodeWindow.hpp"
#include "utility/Standard.hpp"

#include "node/Node.hpp"

#include <QApplication>
#include <QDesktopWidget>
#include <QVariant>

NodeWindow::NodeWindow(QSharedPointer<Node> node, QWidget *parent)
	: QWidget(parent)
	, mNode(node)
{
	OC_METHODGATE();
}



NodeWindow::~NodeWindow()
{
	OC_METHODGATE();
	//qDebug()<<"####################################### NODEWINDOW DTOR";

}


///////////////////////////////////////////////////////////////////////////////


void NodeWindow::loadWindowGeometry()
{
#ifdef Q_OS_ANDROID
	showFullScreen();
#else
	QByteArray geometry;
	if(!mNode.isNull()) {
		Settings &s=mNode->settings();
		geometry = s.getCustomSettingByteArray("geometry", QByteArray());
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
		Settings &s=mNode->settings();
		QByteArray geometry = saveGeometry();
		//qDebug()<<"#*#*#*#*#*#*#*#* SAVING GEOMETRY "<<geometry;
		s.setCustomSettingByteArray("geometry", geometry);
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
