#include "PairingDebugWidget.hpp"
#include "ui_PairingDebugWidget.h"


#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"


PairingDebugWidget::PairingDebugWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::PairingDebugWidget)
	, mConfigureHelper("PairingDebugWidget", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


PairingDebugWidget::~PairingDebugWidget()
{
	OC_METHODGATE();
	delete ui;
}


void PairingDebugWidget::configure(QSharedPointer <Node> node)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode=node;
		if(!mNode.isNull()) {
			qDebug()<<"CONFIG";

		}
	}
}


void  PairingDebugWidget::onUpdate()
{
	OC_METHODGATE();
	if(!mNode.isNull()) {
		qDebug()<<"UPDATE";
	}
}

