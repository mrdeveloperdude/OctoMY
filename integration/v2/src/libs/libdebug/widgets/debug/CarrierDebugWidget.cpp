#include "CarrierDebugWidget.hpp"
#include "ui_CarrierDebugWidget.h"

#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"

#include "utility/string/String.hpp"

#include "comms/CommsChannel.hpp"

CarrierDebugWidget::CarrierDebugWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::CarrierDebugWidget)
	, mConfigureHelper("CarrierDebugWidget", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}

CarrierDebugWidget::~CarrierDebugWidget()
{
	OC_METHODGATE();
	delete ui;
}

void CarrierDebugWidget::configure(QSharedPointer <Node> node)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode=node;
		if(!mNode.isNull()) {
			ui->widgetCouriersDebug->configure(mNode);
			auto cc=mNode->comms();
			if(!cc.isNull()) {
				auto car=cc->carrier();
				if(!car.isNull()) {
					if(!connect(car.data(), &CommsCarrier::carrierConnectionStatusChanged, this, [this, car](const bool conn) {
					Q_UNUSED(conn);
						ui->widgetStatusConnectionStarted->setLightOn(car->isConnectionStarted());
						ui->widgetStatusConnectionConnected->setLightOn(car->isConnected());
						ui->widgetStatusConnectionMaintained->setLightOn(car->isConnectionMaintained());
					}, OC_CONTYPE )) {
						qWarning()<<"ERROR: Could not connect";
					}

					if(!connect(car.data(), &CommsCarrier::carrierError, this, [this, car](const QString errorStr) {
					qDebug().noquote().nospace()<<"CommsCarrier error: errorStr="<<errorStr;
						ui->labelCarrierType->setText(errorStr);
					}, OC_CONTYPE )) {
						qWarning()<<"ERROR: Could not connect";
					}

					if(!connect(car.data(), &CommsCarrier::carrierSendingOpportunity, this, [this, car](const quint64 now) {
					qDebug().noquote().nospace()<<"CommsCarrier sending oportunity: now="<<now;
						ui->labelLastSendingOpportunity->setText(utility::string::humanReadableElapsedMS(now));
					}, OC_CONTYPE )) {
						qWarning()<<"ERROR: Could not connect";
					}


				} else {
					qWarning()<<"ERROR: No car";
				}
			} else {
				qWarning()<<"ERROR: No comms";
			}
		}
		/*
		if(!connect(ui->checkBoxUpdateRealtime, &QCheckBox::toggled, this, &ServicesDebugWidget::onRealtimeChanged, OC_CONTYPE )) {
			qWarning()<<"ERROR: Could not connect";
		}
		// We sacrifice quality since this is for debugging purpose onle and we want this to have the least impact on the runtime of non-debug code
		mTimer.setTimerType(Qt::VeryCoarseTimer);
		mTimer.setInterval(1000/15);// 15 FPS
		if(!connect(&mTimer, &QTimer::timeout, this, &ServicesDebugWidget::onTimer, OC_CONTYPE )) {
			qWarning()<<"ERROR: Could not connect";
		}
		onRealtimeChanged(ui->checkBoxUpdateRealtime->isChecked());
		*/
	}
}

