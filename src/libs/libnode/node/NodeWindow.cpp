#include "NodeWindow.hpp"

#include "uptime/MethodGate.hpp"
#include "app/Settings.hpp"
#include "node/Node.hpp"

#include "identity/Identicon.hpp"

#include <QApplication>
#include <QDebug>
#include <QIcon>
#include <QKeyEvent>
#include <QPainter>
#include <QRect>
#include <QScreen>
#include <QVariant>

static const QString GEOMETRY_SETTINGS_KEY("window.geometry");

NodeWindow::NodeWindow(QWidget *parent)
	: QWidget(parent)
	, mNode(nullptr)
	, mWaterMark(QString(":/images/agent_watermark.svg"), this)
{
	OC_METHODGATE();
}


NodeWindow::~NodeWindow()
{
	OC_METHODGATE();
	mNode.clear();
}


bool NodeWindow::shouldRedrawWatermark(const QSize& newSize) {
	if (mWatermarkLastSize.isEmpty()) {
		return true;
	}
	double widthChange = std::abs(mWatermarkLastSize.width() - newSize.width()) / static_cast<double>(mWatermarkLastSize.width());
	double heightChange = std::abs(mWatermarkLastSize.height() - newSize.height()) / static_cast<double>(mWatermarkLastSize.height());
	return (widthChange > 0.1) || (heightChange > 0.1);
}

void NodeWindow::drawWatermark(){
	QPainter painter(this);
	QSize widgetSize = size();
	if (!shouldRedrawWatermark(widgetSize)) {
		painter.drawPixmap(0, 0, mWatermarkCache);
		return;
	}
	QSize svgSize = mWaterMark.defaultSize();
	double widthScale = static_cast<double>(widgetSize.width()) / svgSize.width();
	double heightScale = static_cast<double>(widgetSize.height()) / svgSize.height();
	double scale = qMin(widthScale, heightScale);
	int watermarkWidth = static_cast<int>(svgSize.width() * scale);
	int watermarkHeight = static_cast<int>(svgSize.height() * scale);
	QRect targetRect(0, 0, watermarkWidth, watermarkHeight);
	mWatermarkCache = QPixmap(widgetSize);
	mWatermarkCache.fill(Qt::transparent);
	QPainter cachePainter(&mWatermarkCache);
	mWaterMark.render(&cachePainter, targetRect);
	mWatermarkLastSize = widgetSize;
	painter.drawPixmap(0, 0, mWatermarkCache);
}


void NodeWindow::nodeWindowConfigure(QSharedPointer<Node> node)
{
	OC_METHODGATE();
	// Disconnect old node
	if(!mNode.isNull()) {
// [...]
		mNode.clear();

	}
	// Set new node
	// qDebug()<<"SETTING NODE TO "<<node;
	mNode = node;
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

void NodeWindow::focus()
{
	OC_METHODGATE();
	setWindowState( (windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
	raise();  // for MacOS
	activateWindow(); // for Windows
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
			//TODO: Verify this Qt6 conversion
			//const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
			auto screen = QApplication::primaryScreen();
			const QRect availableGeometry = screen->availableGeometry();
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


void NodeWindow::moveEvent(QMoveEvent *event)
{
	OC_METHODGATE();
	if(nullptr!= event) {
		emit nodeWindowMoved(sharedFromThis());
	}
}



void NodeWindow::paintEvent(QPaintEvent *event) {
	OC_METHODGATE();
	QWidget::paintEvent(event);
	drawWatermark();
}

///////////////////////////////////////////////////////////////////////////////

void NodeWindow::notifyAndroid(QString s)
{
	OC_METHODGATE();
	(void)s;
	//TODO: This crashes with some jni exception stuff. Figure out why
#ifdef Q_OS_ANDROID
	qDebug()<<"QT: Android NOTIFE: "<<s;
	QJniObject::callStaticMethod<void>("org/octomy/Agent/Agent",
			"notify",
			"(Ljava/lang/String;)V",
			QJniObject::fromString(s).object<jstring>());
#endif
}


void NodeWindow::toastAndroid(QString s)
{
	OC_METHODGATE();
	(void)s;
	//TODO: This crashes with some jni exception stuff. Figure out why
#ifdef Q_OS_ANDROID
	qDebug()<<"QT: Android TOAST: "<<s;
	QJniObject::callStaticMethod<void>("org/octomy/Agent/Agent",
			"toast",
			"(Ljava/lang/String;)V",
			QJniObject::fromString(s).object<jstring>());
#endif
}
