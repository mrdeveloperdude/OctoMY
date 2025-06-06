#include "NodeWindow.hpp"

#include "app/Settings.hpp"
#include "components/navigation/ActivityStack.hpp"
#include "identity/Identicon.hpp"
#include "node/Node.hpp"
#include "node/NodeType.hpp"
#include "screen/ScreenManager.hpp"
#include "splash/SplashScreen.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

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
	, mWaterMark(QString(), this)
	, mSplash(OC_NEW SplashScreen())
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
		
		auto typeName = nodeTypeToString(mNode->nodeType()).toLower();
		mWaterMark.load(QString(":/images/%1_watermark.svg").arg(typeName));
		auto splash_time{1000};
		mSplash->configure(this, QString(":/icons/app/window/%1.svg").arg(typeName), splash_time, splash_time/4, 0.7);
		configure();
		QTimer::singleShot((splash_time*3/4), mSplash, &SplashScreen::done);
	} else {
		qWarning()<<"WARNING: No Agent in agent window";
	}


}


QSharedPointer<Node> NodeWindow::node() const
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
	if(!mNode.isNull()) {
		QSharedPointer<Settings> s=mNode->settings();
		auto sm = ScreenManager::instance();
		sm->configure(s);
		auto geometry = sm->restore("node", {WindowType::Primary, this->geometry().size()});
		setGeometry(geometry);
	} else {
		qWarning()<<"ERROR: No node while loading window geometry from settings";
	}
#endif
}

void NodeWindow::saveWindowGeometry()
{
	OC_METHODGATE();
	if(!mNode.isNull()) {
		auto sm = ScreenManager::instance();
		sm->save("node", geometry());
	}
}





void NodeWindow::updateWindowIcon()
{
	OC_METHODGATE();
	PortableID pid;
	if(!mNode.isNull()) {
		auto nid = mNode->nodeIdentity();
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


void NodeWindow::setActivityStack(ActivityStack *activityStack){
	OC_METHODGATE();
	mActivityStack = activityStack;
}




void NodeWindow::keyReleaseEvent(QKeyEvent *e)
{
	OC_METHODGATE();
	if(mActivityStack){
		if(Qt::Key_Back==e->key()) {
			mActivityStack->pop();
			e->accept();
		}
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


QString NodeWindow::popPage(){
	OC_METHODGATE();
	const auto ret = mActivityStack->pop();
	if(mDebug){
		qDebug()<<"POP PAGE " << ret;
	}
	return ret;
}


bool NodeWindow::pushPage(const QString &title, const QStringList arguments){
	OC_METHODGATE();
	const auto ret = mActivityStack->push(title, arguments);
	if(mDebug){
		qDebug()<<"PUSH PAGE " << title << arguments << ret;
	}
	return ret;
}


QString NodeWindow::swapPage(const QString &title){
	OC_METHODGATE();
	const auto ret = mActivityStack->swap(title);
	if(mDebug){
		qDebug()<<"SWAP PAGE " << ret << "-->" << title;
	}
	return ret;
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
