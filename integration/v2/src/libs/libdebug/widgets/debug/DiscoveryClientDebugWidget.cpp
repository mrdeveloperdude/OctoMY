#include "DiscoveryClientDebugWidget.hpp"
#include "ui_DiscoveryClientDebugWidget.h"

#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"

#include "utility/string/String.hpp"

#include "discovery/DiscoveryClient.hpp"


DiscoveryClientDebugWidget::DiscoveryClientDebugWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::DiscoveryClientDebugWidget)
	, mConfigureHelper("DiscoveryClientDebugWidget", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


DiscoveryClientDebugWidget::~DiscoveryClientDebugWidget()
{
	OC_METHODGATE();
	delete ui;
}


void DiscoveryClientDebugWidget::configure(QSharedPointer <Node> node)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode=node;
		if(!mNode.isNull()) {
			auto dc=mNode->discoveryClient();
			if(!dc.isNull()) {
				ui->widgetRateCalculatorRX->configure(&dc->mRXRate);
				ui->widgetRateCalculatorTX->configure(&dc->mTXRate);
				if(!connect(dc.data(), &DiscoveryClient::discoverRequest, ui->widgetStatusTX, [this]() {
				ui->widgetStatusTX->blink(Constants::OC_STATUS_BLINK_TIME);
				}, OC_CONTYPE )) {
					qWarning()<<"ERROR: Could not connect";
				}
				if(!connect(dc.data(), &DiscoveryClient::discoverResponse, ui->widgetStatusRX, [this](const bool ok) {
				ui->widgetStatusRX->setLightColor(ok?LightWidget::sDefaultOKColor:LightWidget::sDefaultErrorColor);
					ui->widgetStatusRX->blink(Constants::OC_STATUS_BLINK_TIME);
				}, OC_CONTYPE )) {
					qWarning()<<"ERROR: Could not connect";
				}
				update();
			} else {
				qWarning()<<"ERROR: No discovery client";
			}

		}
		// We sacrifice quality since this is for debugging purpose onle and we want this to have the least impact on the runtime of non-debug code
		mTimer.setTimerType(Qt::VeryCoarseTimer);
		mTimer.setInterval(1000);// 1 FPS
		mTimer.setSingleShot(false);
		if(!connect(&mTimer, &QTimer::timeout, this, &DiscoveryClientDebugWidget::onUpdate, OC_CONTYPE )) {
			qWarning()<<"ERROR: Could not connect";
		}
		//mTimer.start();
	}
}


void  DiscoveryClientDebugWidget::onUpdate()
{
	OC_METHODGATE();
	qDebug()<<"UPDATE";
	if(!mNode.isNull()) {
		ui->widgetRateCalculatorRX->update();
		ui->widgetRateCalculatorTX->update();
		auto dc=mNode->discoveryClient();
		if(!dc.isNull()) {
			ui->widgetStatusActive->setLightOn(dc->isActive());
			ui->labelServerURL->setText(dc->URL().toString());
			ui->labelLastZooPairing->setText(utility::string::humanReadableElapsedMS(dc->lastZooPairTime()));
			ui->pushButtonLogging->setChecked(dc->isLogging());
		} else {
			ui->widgetStatusActive->setLightOn(false);
			ui->labelServerURL->setText("N/A");
			ui->labelLastZooPairing->setText("N/A");
			qWarning()<<"ERROR: No discovery client";
		}
	}
}


void DiscoveryClientDebugWidget::on_pushButtonLogging_toggled(bool logging)
{
	OC_METHODGATE();
	if(!mNode.isNull()) {
		auto dc=mNode->discoveryClient();
		if(!dc.isNull()) {
			dc->setLogging(logging);
		}
	}
}


void DiscoveryClientDebugWidget::on_pushButtonDiscover_clicked()
{
	OC_METHODGATE();
	if(!mNode.isNull()) {
		auto dc=mNode->discoveryClient();
		if(!dc.isNull()) {
			dc->discover();
		}
	}
}
