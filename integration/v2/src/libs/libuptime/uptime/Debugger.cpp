#include "Debugger.hpp"
#include "ui_Debugger.h"

#include "uptime/MethodGate.hpp"

Debugger::Debugger(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Debugger)
{
	OC_METHODGATE();
	ui->setupUi(this);
	setEnabled(false);
}

Debugger::~Debugger()
{
	OC_METHODGATE();
	delete ui;
}


void Debugger::configure(QSharedPointer <Node> node)
{
	OC_METHODGATE();
	mNode=node;
	setEnabled(!mNode.isNull());
}

void Debugger::on_pushButtonQuitSuccess_clicked()
{
	OC_METHODGATE();
	if(!mNode.isNull()) {
		emit mNode->nodeRequestExit(EXIT_SUCCESS);
	}
}

void Debugger::on_pushButtonQuitFail_clicked()
{
	OC_METHODGATE();
	if(!mNode.isNull()) {
		emit mNode->nodeRequestExit(EXIT_FAILURE);
	}
}

void Debugger::on_pushButtonUnbirth_clicked()
{
	OC_METHODGATE();
	if(!mNode.isNull()) {
		mNode->unbirth();
	}
}
