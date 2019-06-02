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
			auto cc=mNode->comms();
			if(!cc.isNull()) {
				auto car=cc->carrier();
				if(!car.isNull()) {

					ui->widgetRateCalculatorRX->configure(&car->mRXRate);
					ui->widgetRateCalculatorTX->configure(&car->mTXRate);
					ui->labelCarrierType->setText(car->objectName());

					if(!connect(car.data(), &CommsCarrier::carrierReadyRead, this, [this, car]() {
					//qDebug().noquote().nospace()<<"CommsCarrier read ready";
						onUpdate();
					}, OC_CONTYPE )) {
						qWarning()<<"ERROR: Could not connect";
					}

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
					//qDebug().noquote().nospace()<<"CommsCarrier sending opportunity: now="<<now;
						ui->labelLastSendingOpportunity->setText(utility::string::humanReadableElapsedMS(now));
						onUpdate();
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
		else{
			qWarning()<<"WARNING: No node";
		}
	}
}


void  CarrierDebugWidget::onUpdate()
{
	OC_METHODGATE();
	ui->widgetRateCalculatorRX->update();
	ui->widgetRateCalculatorTX->update();
}
