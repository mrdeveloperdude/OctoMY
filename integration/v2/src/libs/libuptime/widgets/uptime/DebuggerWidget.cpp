#include "DebuggerWidget.hpp"
#include "ui_DebuggerWidget.h"

#include "uptime/MethodGate.hpp"

DebuggerWidget::DebuggerWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Debugger)
{
	OC_METHODGATE();
	ui->setupUi(this);
	setEnabled(false);
}

DebuggerWidget::~DebuggerWidget()
{
	OC_METHODGATE();
	delete ui;
}


void DebuggerWidget::configure(QSharedPointer <Node> node)
{
	OC_METHODGATE();
	mNode=node;
	setEnabled(!mNode.isNull());
}

void DebuggerWidget::on_pushButtonQuitSuccess_clicked()
{
	OC_METHODGATE();
	if(!mNode.isNull()) {
		emit mNode->nodeRequestExit(EXIT_SUCCESS);
	}
}

void DebuggerWidget::on_pushButtonQuitFail_clicked()
{
	OC_METHODGATE();
	if(!mNode.isNull()) {
		emit mNode->nodeRequestExit(EXIT_FAILURE);
	}
}

void DebuggerWidget::on_pushButtonUnbirth_clicked()
{
	OC_METHODGATE();
	if(!mNode.isNull()) {
		mNode->unbirth();
	}
}
