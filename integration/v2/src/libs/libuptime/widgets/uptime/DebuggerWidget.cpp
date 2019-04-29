#include "DebuggerWidget.hpp"
#include "ui_DebuggerWidget.h"

#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"
#include "utility/time/HumanTime.hpp"

#include "service/ServiceLevelManager.hpp"


#include "name/AgentNameGenerator.hpp"
#include "name/GenderGenerator.hpp"

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

static void configTryToggleForServiceLevel(TryToggle *tt, const QString serviceLevel, const QSharedPointer<Node> node, const QString offText, const QString goingOnText, const QString onText, const QString goingOffText)
{
	OC_FUNCTIONGATE();
	if(nullptr != tt) {
		if(!node.isNull()) {
			tt->configure(offText, goingOnText, onText, goingOffText);
			if(!QObject::connect(tt, &TryToggle::stateChanged, node.data(), [=](const TryToggleState last, const TryToggleState current) {
			Q_UNUSED(last);
				auto slm=node->serviceLevelManager();
				if(!slm.isNull()) {
					slm->enableLevel(serviceLevel, positive(current), [=](bool ok) {
						tt->setState(saturate(ok?current:last), false);
					});
				} else {
					qWarning()<<"WARNING: Could not switch discovery service, no service manager";
				}
			}, OC_CONTYPE)) {
			}
		} else {
			qWarning()<<"WARNING: Could not switch discovery service, no node";
		}
	} else {
		qWarning()<<"WARNING: No try toggle in configTryToggleForServiceLevel()";
	}
}

void DebuggerWidget::configure(QSharedPointer <Node> node)
{
	OC_METHODGATE();
	mNode=node;
	setEnabled(!mNode.isNull());
	ui->widgetKeyStore->configure(mNode);
	ui->widgetLocalIdentityDebug->configure(mNode);
	ui->widgetPairingList->configure(mNode);
	ui->widgetLocalAddresses->configure(mNode);
	ui->widgetNetworkSettings->configure(mNode?mNode->localAddressList():nullptr);

	configTryToggleForServiceLevel(ui->tryToggleDiscovery, "Discovery", mNode, "Activate disovery", "Discovery activating", "Deactivate discovery", "Discovery deactivating");

	configTryToggleForServiceLevel(ui->tryToggleAlways, "Always", mNode, "Activate", "Activating", "Deactivate", "Deactivating");
}

void DebuggerWidget::showEvent(QShowEvent *event)
{
	OC_METHODGATE();
	Q_UNUSED(event);
	emit visibilityChanged(true);
}

void DebuggerWidget::hideEvent(QHideEvent *event)
{
	OC_METHODGATE();
	Q_UNUSED(event);
	emit visibilityChanged(false);
}


void DebuggerWidget::on_pushButtonQuitSuccess_clicked()
{
	OC_METHODGATE();
	if(!mNode.isNull()) {
		emit mNode->nodeRequestExit(EXIT_SUCCESS);
	} else {
		qWarning()<<"WARNING: Could not emit successfull quit, no node";
	}
}


void DebuggerWidget::on_pushButtonQuitFail_clicked()
{
	OC_METHODGATE();
	if(!mNode.isNull()) {
		emit mNode->nodeRequestExit(EXIT_FAILURE);
	} else {
		qWarning()<<"WARNING: Could not emit failure quit, no node";
	}
}


void DebuggerWidget::on_pushButtonUnbirth_clicked()
{
	OC_METHODGATE();
	if(!mNode.isNull()) {
		mNode->unbirth();
	} else {
		qWarning()<<"WARNING: Could not unbirth, no node";
	}
}


void DebuggerWidget::on_pushButtonBirth_clicked()
{
	OC_METHODGATE();
	if(!mNode.isNull()) {
		QSharedPointer<KeyStore> keystore=mNode->keyStore();
		if(!keystore.isNull()) {
			keystore->clear([=](ASEvent<QVariantMap> &ase1) {
				Q_UNUSED(ase1);
				keystore->setBootstrapEnabled(true);
				keystore->synchronize([=](ASEvent<QVariantMap> &ase2) {
					Q_UNUSED(ase2);
					QSharedPointer<Key> key=keystore->localKey();
					if(!key.isNull()) {
						QVariantMap map;
						const quint64 now=utility::time::currentMsecsSinceEpoch<quint64>();
						AgentNameGenerator ang;
						GenderGenerator gg;
						map["key"]=key->toVariantMap(true);
						map["name"]=ang.generate();
						map["gender"]=gg.generate();
						map["role"]=nodeRoleToString(mNode->nodeRole());
						map["type"]=nodeTypeToString(mNode->nodeType());
						map["birthDate"]=utility::time::msToVariant(now);
						qDebug()<<"Creating new identity: "<<map;
						mNode->setNodeIdentity(map);
					}
				});
			});

		}
	} else {
		qWarning()<<"WARNING: Could not birth, no node";
	}
}



void DebuggerWidget::on_pushButtonActivate_toggled(bool checked)
{
	OC_METHODGATE();
	if(!mNode.isNull()) {
		mNode->appActivate(checked);
	} else {
		qWarning()<<"WARNING: Could not switch discovery service, no node";
	}
}

