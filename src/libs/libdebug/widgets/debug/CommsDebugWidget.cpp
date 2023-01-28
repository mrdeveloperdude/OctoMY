#include "CommsDebugWidget.hpp"
#include "ui_CommsDebugWidget.h"

#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"


#include "comms/CommsChannel.hpp"

CommsDebugWidget::CommsDebugWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::CommsDebugWidget)
	, mConfigureHelper("CommsDebugWidget", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


CommsDebugWidget::~CommsDebugWidget()
{
	OC_METHODGATE();
	delete ui;
}


void CommsDebugWidget::configure(QSharedPointer <Node> node)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode=node;
		if(!mNode.isNull()) {
			ui->widgetCouriersDebug->configure(mNode);
			auto cc=mNode->comms();
			if(!cc.isNull()) {
				if(!connect(cc.data(), &CommsChannel::commsConnectionStatusChanged, this, [this](const bool isConnected, const bool needsConnection) {
				qDebug().noquote().nospace()<<"Connection status changed: isConnected="<<isConnected<<", isConnected="<<needsConnection;

					ui->widgetStatusConnectionActual->setLightOn(isConnected);
					ui->widgetStatusConnectionWanted->setLightOn(needsConnection);

				}, OC_CONTYPE_NON_UNIQUE)) {
					qWarning()<<"ERROR: Could not connect";
				}
			}
		}
		else{
			qWarning()<<"WARNING: No node";
		}
		/*
		if(!connect(ui->checkBoxUpdateRealtime, &QCheckBox::toggled, this, &ServicesDebugWidget::onRealtimeChanged, OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect";
		}
		// We sacrifice quality since this is for debugging purpose onle and we want this to have the least impact on the runtime of non-debug code
		mTimer.setTimerType(Qt::VeryCoarseTimer);
		mTimer.setInterval(1000/15);// 15 FPS
		if(!connect(&mTimer, &QTimer::timeout, this, &ServicesDebugWidget::onTimer, OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect";
		}
		onRealtimeChanged(ui->checkBoxUpdateRealtime->isChecked());
		*/
	}
}

