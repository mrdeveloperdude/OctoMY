#include "NodeNavigation.hpp"
#include "ui_NodeNavigation.h"

#include "uptime/MethodGate.hpp"

NodeNavigation::NodeNavigation(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::NodeNavigation)
{
	OC_METHODGATE();
	ui->setupUi(this);
	// Start by hiding the whole thing
	setState("", false, false, false);
}

NodeNavigation::~NodeNavigation()
{
	OC_METHODGATE();
	delete ui;
}


void NodeNavigation::setState(QString title, bool showBar, bool showBack, bool showMenu){
	OC_METHODGATE();
	qDebug()<<"NodeNavigation::setState(title=" << title << ", showBar="<<showBar << ", showBack=" << showBack << ", showMenu=" << showMenu << ")";
	if(showBar){
		ui->labelTitle->setText(title);
		ui->pushButtonBack->setVisible(showBack);
		ui->pushButtonMenu->setVisible(showMenu);
	}
	setVisible(showBar);
}


QPoint NodeNavigation::menuPos(){
	OC_METHODGATE();
	return ui->pushButtonMenu->pos();
}


void NodeNavigation::configure(QSharedPointer<Node> node){
	OC_METHODGATE();
	ui->pushButtonDebugger->configure(node, true);
}


void NodeNavigation::onNavigateBack(){
	OC_METHODGATE();
	qDebug()<<"NodeNavigation::onNavigateBack()";
	emit navigateBack();
}


void NodeNavigation::onOpenMenu(){
	OC_METHODGATE();
	qDebug()<<"NodeNavigation::onOpenMenu()";
	emit openMenu();
}
