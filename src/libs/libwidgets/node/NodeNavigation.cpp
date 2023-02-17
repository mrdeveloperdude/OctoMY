#include "NodeNavigation.hpp"
#include "ui_NodeNavigation.h"

#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"

NodeNavigation::NodeNavigation(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::NodeNavigation)
{
	OC_METHODGATE();
	ui->setupUi(this);
	// Start by hiding the whole thing
	setState("", false, false, false);
	if(!connect(ui->pushButtonBack, &QPushButton::clicked, this, &NodeNavigation::backClicked, OC_CONTYPE)){
		qWarning()<<"Could not connect NodeNavigation back button";
	}
	if(!connect(ui->pushButtonMenu, &QPushButton::clicked, this, &NodeNavigation::menuClicked, OC_CONTYPE)){
		qWarning()<<"Could not connect NodeNavigation menu button";
	}
}

NodeNavigation::~NodeNavigation()
{
	OC_METHODGATE();
	delete ui;
}


void NodeNavigation::setState(QString title, bool showBar, bool showBack, bool showMenu){
	OC_METHODGATE();
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
