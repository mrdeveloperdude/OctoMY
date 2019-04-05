#include "NodeWindow.hpp"

#include "uptime/MethodGate.hpp"
#include "app/Settings.hpp"
#include "node/Node.hpp"

#include "identity/Identicon.hpp"

#include <QDebug>
#include <QRect>
#include <QApplication>
#include <QDesktopWidget>
#include <QVariant>
#include <QIcon>

static const QString GEOMETRY_SETTINGS_KEY("window.geometry");

NodeWindow::NodeWindow(QWidget *parent)
	: QWidget(parent)
	, mNode(nullptr)
{
	OC_METHODGATE();
}


NodeWindow::~NodeWindow()
{
	OC_METHODGATE();
	mNode.clear();
}



void NodeWindow::nodeWindowConfigure(QSharedPointer<Node> node)
{
	OC_METHODGATE();
	// Dissconnect old node
	if(!mNode.isNull()) {
// [...]
		mNode.clear();

	}
	// Set new node
	// qDebug()<<"SETTING NODE TO "<<node;
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


void NodeWindow::nodeWindowRequestExit(const int returnValue)
{
	OC_METHODGATE();
	//qDebug()<<"nodeWindowRequestExit(returnValue="<<returnValue<<")";
	auto n=node();
	if(!n.isNull()) {
		emit n->nodeRequestExit(returnValue);
	} else {
		qWarning()<<"ERROR: No node";
	}

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
		geometry = s->getCustomSettingByteArray(GEOMETRY_SETTINGS_KEY, QByteArray());
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
	} else {
		qWarning()<<"ERROR: No node while loading window geometry from settings";
	}
#endif
}

void NodeWindow::saveWindowGeometry()
{
	OC_METHODGATE();
	if(!mNode.isNull()) {
		QSharedPointer<Settings> s=mNode->settings();
		QByteArray geometry = saveGeometry();
		//qDebug()<<"#*#*#*#*#*#*#*#* SAVING GEOMETRY "<<geometry;
		s->setCustomSettingByteArray(GEOMETRY_SETTINGS_KEY, geometry);
	}
}





void NodeWindow::updateWindowIcon()
{
	OC_METHODGATE();
	PortableID pid;
	if(!mNode.isNull()) {
		QSharedPointer<Associate>nid=mNode->nodeIdentity();
		if(!nid.isNull()) {
			pid=nid->toPortableID();
		}
	}
	//Set our custom identicon as window icon
	Identicon id(pid);
	QIcon icon;
	icon.addPixmap(id.pixmap(512,512));
	setWindowIcon(icon);
}





void NodeWindow::keyReleaseEvent(QKeyEvent *e)
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



void NodeWindow::closeEvent(QCloseEvent *event)
{
	OC_METHODGATE();
	Q_UNUSED(event);
	nodeWindowRequestExit(EXIT_SUCCESS);
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
