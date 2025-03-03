#include "HubWindow.hpp"

#include "ui_HubWindow.h"

#include "Hub.hpp"

#include "comms/CommsChannel.hpp"
#include "comms/CommsSession.hpp"
#include "hub/NodesManagementWidget.hpp"
#include "log/LogStorage.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"


#include <QHostInfo>
#include <QNetworkInterface>
#include <QProcess>
#include <QScrollBar>
#include <QSplashScreen>
#include <QString>
#include <QStringList>
#include <QTextStream>


HubWindow::HubWindow(QWidget *parent)
	: NodeWindow(parent)
	, ui(OC_NEW Ui::HubWindow)

{
	OC_METHODGATE();
	setObjectName("HubWindow");
	ui->setupUi(this);
	mWaterMark.load(QString(":/images/hub_watermark.svg"));
}


HubWindow::~HubWindow()
{
	OC_METHODGATE();
	saveWindowGeometry();
	delete ui;
	ui=nullptr;
}



void HubWindow::configure()
{
	OC_METHODGATE();
	auto h=hub();
	if(!h.isNull()) {
		loadWindowGeometry();
		//ui->widgetDebuggerButton->configure(node(), true);
		updateWindowIcon();
		//ui->widgetPairing->configure(node());
		QSharedPointer<Settings>  s=h->settings();
		auto settings=s.data();
		restoreGeometry(settings->getCustomSettingByteArray("window.geometry"));
		QSplashScreen *splash=OC_NEW QSplashScreen(this->screen(), QPixmap(":/images/hub_butterfly.svg"), Qt::WindowStaysOnTopHint);
		splash->show();
		ui->widgetPairing->configure(h);
		/*
		QAbstractItemModel *data = OC_NEW ClientModel(hub->getComms()->getClients(), this);
		ui->widgetIncommingNodes->configure("Icons","hubwindiow-clients-list");
		ui->widgetIncommingNodes->setModel(data);
		*/
		ui->tabWidget->setEnabled(true);
		ui->tabWidget->setCurrentWidget(ui->tabDevelopment);
		ui->tabDevelopment->configure(s, hub());
		ui->tabUtilities->configure(s, hub());
		ui->tabNodes->configure(s);
		

#ifdef OC_USE_LIB_EXT_OPENCL
		if(!connect(ui->openGLWidgetCLGLView, SIGNAL(glInitialized()), this, SLOT(onGLWidgetInitialized()), OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect";
		}
#endif
// OC_USE_LIB_EXT_OPENCL
		h->comms()->hookCommsSignals(*this,true);
		appendLog("SETTING UP PLAN EDITOR");
		ui->widgetPlanEditor->configure("hub.plan");
		//updateClientsList();
		appendLog("READY");
		QTimer::singleShot(1500,splash,SLOT(deleteLater()));
		splash=nullptr;
		ui->tabDevelopment->updateIdentityWidgets();
	} else {
		qWarning()<<"WARNING: No Agent in agent window configure";
	}
}


QSharedPointer<Hub> HubWindow::hub()
{
	OC_METHODGATE();
	QSharedPointer<Node> n=node();
	if(n.isNull()) {
		qWarning()<<"WARNING: No node in hub()";
	}
	return qSharedPointerCast<Hub> (n);
}


void HubWindow::appendLog(const QString& text)
{
	OC_METHODGATE();
	auto logStorage = mHub->logStorage();
	if(logStorage){
		logStorage->appendLog(text);
	}
}

void HubWindow::on_pushButtonSendData_clicked()
{
	OC_METHODGATE();
	//TODO: Implement or remove
}




#ifdef OC_USE_LIB_EXT_OPENCL

void HubWindow::onGLWidgetInitialized()
{
	qDebug()<<"INIT CL ----- ";
	HelloGLCLViewRenderer *rendrer=OC_NEW HelloGLCLViewRenderer();
	if(nullptr!=rendrer) {
		rendrer->initialize(ui->openGLWidgetCLGLView->sharingGLContext());
		ui->openGLWidgetCLGLView->setRenderer(rendrer);

		if(!connect(ui->pushButtonCLGLDisplay, &QPushButton::toggled,ui->openGLWidgetCLGLView,&CLGLView::onDisplayToggle,OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect";
		}
		if(!connect(ui->pushButtonCLGLRender, &QPushButton::toggled,ui->openGLWidgetCLGLView,&CLGLView::onRenderToggle,OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect";
		}
	} else {
		qWarning()<<"ERROR: could not allocate rendrer";
	}
}

#endif
// OC_USE_LIB_EXT_OPENCL


// CommsChannel slots
///////////////////////////////////////////

void HubWindow::onCommsError(QString msg)
{
	OC_METHODGATE();
	if("Unable to send a message"==msg) {

	} else {
		appendLog("GOT ERROR: "+msg);
	}
}


void HubWindow::onCommsClientAdded(CommsSession *c)
{
	OC_METHODGATE();
	if(0!=c) {
		appendLog("CLIENT ADDED: "+c->summary());
		ui->tabNodes->update();
	}
}


void HubWindow::onCommsConnectionStatusChanged(const bool isConnected, const bool needsConnection)
{
	OC_METHODGATE();
	qDebug()<<"connection state changed: "<<isConnected<<needsConnection;
}


void HubWindow::on_pushButtonTest_clicked()
{
	OC_METHODGATE();
	qDebug()<<"TEST BUTTON PRESSED";
}


void HubWindow::on_lineEditIdenticonID_textChanged(const QString &)
{
	OC_METHODGATE();
	ui->tabDevelopment->updateIdentityWidgets();
}


