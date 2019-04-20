#include "DebuggerWidget.hpp"
#include "ui_DebuggerWidget.h"

#include "uptime/MethodGate.hpp"
#include "utility/time/HumanTime.hpp"

#include "service/ServiceManager.hpp"


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

// NOTE: This is experimental at best.
void DebuggerWidget::on_pushButtonDiscoveryService_toggled(bool checked)
{
	OC_METHODGATE();
	if(!mNode.isNull()) {
		auto sm=mNode->serviceManager();
		if(!sm.isNull()) {
			qDebug()<<"Switching discovery to "<< (checked?"ON":"OFF");
			sm->changeActivation(QSet<QString>{"discovery"}, checked, [this, sm](bool ok){
				Q_UNUSED(ok);
				ui->pushButtonBirth->setChecked(sm->activatedWanted("discovery"));
			});
		} else {
			qWarning()<<"WARNING: Could not switch discovery service, no service manager";
		}

	} else {
		qWarning()<<"WARNING: Could not switch discovery service, no node";
	}
}
