#include "DebuggerWidget.hpp"
#include "ui_DebuggerWidget.h"

#include "uptime/MethodGate.hpp"
#include "utility/time/HumanTime.hpp"


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
	ui->widgetLocalIdentityDebug->configure(mNode);
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
						map["role"]=nodeRoleToString(ROLE_AGENT);
						map["type"]=nodeTypeToString(TYPE_AGENT);
						map["birthDate"]=utility::time::msToVariant(now);
						qDebug()<<"Creating new identity: "<<map;
						mNode->setNodeIdentity(map);
					}
				});
			});

		}
	}

}
