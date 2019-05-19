#include "PairingDebugWidget.hpp"
#include "ui_PairingDebugWidget.h"


#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"

#include "widgets/pairing/PairingTrustWidget.hpp"


PairingDebugWidget::PairingDebugWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::PairingDebugWidget)
	, mConfigureHelper("PairingDebugWidget", true, false, false, true, false)
	, mTrustWidget(OC_NEW PairingTrustWidget())
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
			if(!mTrustWidget.isNull()) {
				mTrustWidget->configure(mNode);
				if(!connect(mTrustWidget.data(), &PairingTrustWidget::editComplete, this, [this](TrustLevel level, bool save) {
				qDebug()<<"EDIT COMPLETE!!!!!!!!!!!!!!!!!!!!!!!!!";
				}, OC_CONTYPE)) {
					qWarning()<<"ERROR: Could not connect";
				}

				if(!connect(mTrustWidget.data(), &PairingTrustWidget::remove, this, [this]() {
				qDebug()<<"REMOVE!!!!!!!!!!!!!!!!!!!!!!!!!";
				}, OC_CONTYPE)) {
					qWarning()<<"ERROR: Could not connect";
				}
			}
			ui->widgetPairingList->configure(mNode);
			if(!connect(ui->widgetPairingList, &PairingListWidget::startEdit, this, [this](const QString id) {
			auto addressBook=mNode->addressBook();
				if(!addressBook.isNull()) {
					auto ass=addressBook->associateByID(id);
					if(!ass.isNull()) {
						mTrustWidget->startEdit(ass);
						mTrustWidget->show();
					} else {
						qWarning()<<"ERROR: No ass";
					}
				}
			}, OC_CONTYPE)) {
				qWarning()<<"ERROR: Could not connect";
			}
		}
	}
}


void  PairingDebugWidget::onUpdate()
{
	OC_METHODGATE();
	if(!mNode.isNull()) {

	}
}

