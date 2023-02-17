#include "AgentWindow.hpp"
#include "ui_AgentWindow.h"

#include "agent/Agent.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/New.hpp"
#include "node/NodeNavigation.hpp"


#include <QDebug>
#include <QMenu>
#include <QAction>

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#endif


AgentWindow::AgentWindow(QWidget *parent)
	: NodeWindow(parent)
	, ui(OC_NEW Ui::AgentWindow)
	, mMenu(OC_NEW QMenu(tr("Menu"), this))
	, mQuitAction{nullptr}
	, mStartCameraPairingAction{nullptr}
	, mStartNormalPairingAction{nullptr}
	, mHardwareAction{nullptr}
	, mPlanEditorAction{nullptr}
	, mToggleOnlineAction{nullptr}
	
	, mShowFaceAction{nullptr}
{
	OC_METHODGATE();
	ui->setupUi(this);
	prepareMenu();
	prepareNavigation();
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
		ui->widgetNavigation->configure(node());
		updateWindowIcon();
		ui->widgetNetworkSettings->configure(a?a->localAddressList():nullptr);
		
		//ui->widgetPairing->configure(node());
		//ui->wi
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


//////////////////


void AgentWindow::prepareNavigation(){
	OC_METHODGATE();
	if(!connect(ui->widgetNavigation, &NodeNavigation::backClicked, this, &AgentWindow::onBackClicked, OC_CONTYPE)){
		qWarning()<<"Could not connect to node navigation back button";
	}
	if(!connect(ui->widgetNavigation, &NodeNavigation::menuClicked, this, &AgentWindow::onMenuClicked, OC_CONTYPE)){
		qWarning()<<"Could not connect to node navigation menu button";
	}
	ui->widgetNavigation->setState("OctoMY™ Agent", true, true, true);
	
	
	addPage(ui->pageConfirmQuit, true, true, false);
	addPage(ui->pageQuitting, false, false, false);
	
}

void AgentWindow::addPage(QWidget *widget, bool headerEnabled, bool backEnabled, bool menuEnabled){
	auto name=widget->objectName();
	//auto name=widget->objectName();
	mPageMap[name]=Page(widget, headerEnabled, backEnabled, menuEnabled);
}

void AgentWindow::setCurrentPage(QString name)
{
	OC_METHODGATE();
	if(mPageMap.contains(name)){
		auto &page=mPageMap[name];
		if(page.widget == ui->pageConfirmQuit){
			qDebug()<<"Handling config quit page";
		}
		else if(page.widget == ui->pageQuitting){
			qDebug()<<"Handling quitting page";
		}
		else{
			qDebug()<<"No special handling for "<<page.widget->objectName();
		}
		//ui->widgetNavigation->
		ui->stackedWidget->setCurrentWidget(page.widget);
	}else{
		qWarning()<<"Failed to go to unknown page"<<name;
	}
	/*
	const QWidget *cur=curr;
	if(ui->pagePairing == cur) {
		if(!mAgent.isNull()) {
			mAgent->updateDiscoveryClient();
		}
		ui->widgetPairing->reset();
	} else if (ui->pageDelivery == cur) {
		ui->widgetDelivery->reset();
	} else if (ui->pageHardware == cur) {
		ui->widgetHardware->reset();
	} else if (ui->pageBirthCertificate == cur) {
		PortableID pid;
		pid.setType(NodeType::TYPE_AGENT);
		if(!mAgent.isNull()) {
			QSharedPointer<Associate> myAss=mAgent->nodeIdentity();
			if(!myAss.isNull()) {
				pid=myAss->toPortableID();
			} else {
				qWarning()<<"ERROR: No ass";
			}
		}
		ui->widgetBirthCertificate->setPortableID(pid);
	} else if (ui->pageRunning == cur) {
	
	}
	QWidget *last=ui->stackedWidget->currentWidget();
	qDebug()<<"Changing current page from "<<last<<" to "<<cur;
	*/
	
}


void AgentWindow::onBackClicked(){
	OC_METHODGATE();
	qWarning()<<"BACK";
}


void AgentWindow::onMenuClicked(){
	OC_METHODGATE();
	qWarning()<<"MENU";
	mMenu->exec(mapToGlobal(ui->widgetNavigation->menuPos()));
}

void AgentWindow::onNotImplementedYet(){
	OC_METHODGATE();
	qWarning()<<"NOT IMPLEMENTED YET";
}

void AgentWindow::onStartQuitApplication(){
	OC_METHODGATE();
	qWarning()<<"QUIT";
	setCurrentPage("ConfirmQuit");
}


void AgentWindow::onStartCameraPairing(){
	OC_METHODGATE();
	onNotImplementedYet();
}
void AgentWindow::onStartNormalPairing(){
	OC_METHODGATE();
	onNotImplementedYet();
}

void AgentWindow::onConfigureHardware(){
	OC_METHODGATE();
	onNotImplementedYet();
}

void AgentWindow::onPlanEditor(){
	OC_METHODGATE();
	onNotImplementedYet();
}

void AgentWindow::onOnlineChanged(){
	OC_METHODGATE();
	onNotImplementedYet();
}




void AgentWindow::onFaceVisibilityChanged(){
	OC_METHODGATE();
	onNotImplementedYet();
}



QAction *AgentWindow::addMenuItem(QString title, QString icon, QString toolTip, void (AgentWindow::*method)() , bool checkable){
	OC_METHODGATE();
	QAction *action=OC_NEW QAction(title, this);
	action->setCheckable(checkable);
	if(!(connect(action, &QAction::triggered, this, method, OC_CONTYPE))){
		qWarning()<<"Could not connect action method for"<<action->text()<<"to menu";
	}else{
		action->setStatusTip(toolTip);
		action->setIcon(QIcon(icon));
		mMenu->addAction(action);
	}
	return action;
}


void AgentWindow::prepareMenu( )
{
	OC_METHODGATE();
	mQuitAction=addMenuItem(tr("Exit"), ":/icons/no.svg", tr("Terminate execution of this Agent"), &AgentWindow::onStartQuitApplication );
	mStartCameraPairingAction=addMenuItem(tr("Camera Pairing"), ":/icons/eye.svg", tr("Start Camera Pairing"), &AgentWindow::onStartCameraPairing );
	mStartNormalPairingAction=addMenuItem(tr("Normal Pairing"), ":/icons/pair.svg", tr("Start Normal Pairing"), &AgentWindow::onStartNormalPairing );
	mHardwareAction=addMenuItem(tr("Configure Hardware"), ":/icons/actuator_control.svg", tr("Connect to the body of the robot"), &AgentWindow::onConfigureHardware );
	mPlanEditorAction=addMenuItem(tr("Plan Editor"), ":/icons/mandate.svg", tr("Open the Plan editor"), &AgentWindow::onPlanEditor );
	mToggleOnlineAction=addMenuItem(tr("Go Online"), ":/icons/on.svg", tr("Toggle availability of Agent"), &AgentWindow::onOnlineChanged, true);
	mShowFaceAction=addMenuItem(tr("Show face"), ":/icons/on.svg", tr("Show the Agent's face in main screen"), &AgentWindow::onFaceVisibilityChanged, true);
	
	
	
	
	
	
	/*
	  
	  
	  
	// Online button
	//////////////////
	mShowOnlineButtonAction->setStatusTip(tr("Show connect button in main screen"));
	mShowOnlineButtonAction->setCheckable(true);
	connect(mShowOnlineButtonAction, &QAction::triggered, this, &AgentWindow::onOnlineButtonVisibilityChanged);
	mMenu.addAction(mShowOnlineButtonAction);
	
	// Show Face
	//////////////////
	
	
	// Show Log scroll
	//////////////////
	mShowLogAction->setStatusTip(tr("Show log in main screen"));
	mShowLogAction->setCheckable(true);
	connect(mShowLogAction, &QAction::triggered, this, &AgentWindow::onLogVisibilityChanged);
	mMenu.addAction(mShowLogAction);
	
	// Show Stats
	//////////////////
	mShowStatsAction->setStatusTip(tr("Show stats in main screen"));
	mShowStatsAction->setCheckable(true);
	connect(mShowStatsAction, &QAction::triggered, this, &AgentWindow::onStatsVisibilityChanged);
	mMenu.addAction(mShowStatsAction);
	
	// Show birth certificate
	///////////////////////////
	mShowBirthCertificateAction->setStatusTip(tr("Show the birth certificate of this agent"));
	mShowBirthCertificateAction->setIcon(QIcon(":/icons/certificate.svg"));
	connect(mShowBirthCertificateAction, &QAction::triggered, this, &AgentWindow::onStartShowBirthCertificate);
	mMenu.addAction(mShowBirthCertificateAction);
	
	// Unbirth
	//////////////////
	mUnbirthAction->setStatusTip(tr("Delete the identity of this agent to restart birth"));
	mUnbirthAction->setIcon(QIcon(":/icons/kill.svg"));
	connect(mUnbirthAction, &QAction::triggered, this, &AgentWindow::onStartUnbirth);
	mMenu.addAction(mUnbirthAction);
	
	
	
	*/
}


