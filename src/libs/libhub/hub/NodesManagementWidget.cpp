#include "NodesManagementWidget.hpp"
#include "app/launcher/AppLauncher.hpp"
#include "ui_NodesManagementWidget.h"

#include <QFileInfo>
#include <QMenu>
#include <QMessageBox>
#include <QMutex>
#include <QProcess>
#include <QRandomGenerator>
#include <QString>



#include "app/Settings.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"



NodesManagementWidget::NodesManagementWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::NodesManagementWidget)
	, mAddLocalMenu{OC_NEW QMenu(this)}
	, mNodeFactory("hub-")
{
	OC_METHODGATE();
	ui->setupUi(this);
	mSummaryTimer.setInterval(100);
	mAddLocalMenu->addAction(ui->actionAddAgent);
	mAddLocalMenu->addAction(ui->actionAddRemote);
}


void NodesManagementWidget::showAddLocal(){
	mAddLocalMenu->popup(QCursor::pos());
}

NodesManagementWidget::~NodesManagementWidget()
{
	OC_METHODGATE();
	delete ui;
}


void NodesManagementWidget::configure(QSharedPointer<Settings>  settings){
	OC_METHODGATE();
	ui->widgetActiveNodes->configure(settings, MultiView::LIST_KEY, "active_nodes");
	ui->widgetIncommingNodes->configure(settings, MultiView::LIST_KEY, "incomming_nodes");
	
	if(!connect(&mSummaryTimer,SIGNAL(timeout()),this,SLOT(onSummaryTimer()), OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
	
	/*
	QAbstractItemModel *data = OC_NEW ClientModel(hub->getComms()->getClients(), this);
	ui->widgetIncommingNodes->configure("Icons","hubwindiow-clients-list");
	ui->widgetIncommingNodes->setModel(data);
*/	
}


void NodesManagementWidget::onSummaryTimer(){
	OC_METHODGATE();
	/*
	auto comms=hub()->comms();
	if(comms.isNull()) {
		ui->plainTextEditSummary->setPlainText("N/A");
	} else {
		
	}
*/
}



void NodesManagementWidget::setListening(bool on){
	OC_METHODGATE();
	on?mSummaryTimer.start():mSummaryTimer.stop();
}



void NodesManagementWidget::addLocalAgent(){
	const auto status = mNodeFactory.forkNodeEventloop(NodeType::TYPE_AGENT);
	if(!status){
		QMessageBox::warning(this, tr("Could not start agent process"), QString("Could not start instance of agent"));
	}
}


void NodesManagementWidget::addLocalRemote(){
	const auto status = mNodeFactory.forkNodeEventloop(NodeType::TYPE_REMOTE);
	if(!status){
		QMessageBox::warning(this, tr("Could not start remote process"), QString("Could not start instance of remote"));
	}
}


void NodesManagementWidget::update(){
	OC_METHODGATE();
	ui->widgetIncommingNodes->update();
}
