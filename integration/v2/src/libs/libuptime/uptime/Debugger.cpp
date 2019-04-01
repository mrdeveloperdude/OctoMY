#include "Debugger.hpp"
#include "ui_Debugger.h"

Debugger::Debugger(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Debugger)
{
	ui->setupUi(this);
	setEnabled(false);
}

Debugger::~Debugger()
{
	delete ui;
}


void Debugger::configure(QSharedPointer <Node> node)
{
	mNode=node;
	setEnabled(!mNode.isNull());
}

void Debugger::on_pushButtonQuitSuccess_clicked()
{
	if(!mNode.isNull()) {
		emit mNode->nodeRequestExit(EXIT_SUCCESS);
	}
}

void Debugger::on_pushButtonQuitFail_clicked()
{
	if(!mNode.isNull()) {
		emit mNode->nodeRequestExit(EXIT_FAILURE);
	}
}
