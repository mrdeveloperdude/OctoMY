#include "KeyStoreDebugWidget.hpp"

#include "node/Node.hpp"
#include "security/KeyStore.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"

#include <QDebug>

KeyStoreDebugWidget::KeyStoreDebugWidget(QWidget *parent)
	: SimpleTableDebugWidget(parent)
	, mConfigureHelper("KeyStoreDebugWidget", true, false, false, true, false)
{
	OC_METHODGATE();
	setObjectName("KeyStoreDebugWidget");
}


KeyStoreDebugWidget::~KeyStoreDebugWidget()
{
	OC_METHODGATE();
}


void KeyStoreDebugWidget::configure(QSharedPointer <Node> node)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		SimpleTableDebugWidget::configure("Key Store");
		mNode=node;
		if(!connect(this, &SimpleTableDebugWidget::saveButtonPressed, this, [this]() {
		qDebug()<<"SAVE PRESSED FOR "<<objectName();
			if(!mNode.isNull()) {
				QSharedPointer<KeyStore> ks=mNode->keyStore();
				if(!ks.isNull()) {
					ks->save();
				}
			}
		},OC_CONTYPE_NON_UNIQUE)) {
			qWarning()<<"ERROR: Could not connect";
		}
		if(!connect(this, &SimpleTableDebugWidget::loadButtonPressed, this, [this]() {
		qDebug()<<"LOAD PRESSED FOR "<<objectName();
			if(!mNode.isNull()) {
				QSharedPointer<KeyStore> ks=mNode->keyStore();
				if(!ks.isNull()) {
					ks->load();
				}
			}
		},OC_CONTYPE_NON_UNIQUE)) {
			qWarning()<<"ERROR: Could not connect";
		}
		if(!connect(this, &SimpleTableDebugWidget::syncButtonPressed, this, [this]() {
		qDebug()<<"SYNC PRESSED FOR "<<objectName();
			if(!mNode.isNull()) {
				QSharedPointer<KeyStore> ks=mNode->keyStore();
				if(!ks.isNull()) {
					ks->synchronize();
				}
			}
		},OC_CONTYPE_NON_UNIQUE)) {
			qWarning()<<"ERROR: Could not connect";
		}
		if(!connect(this, &SimpleTableDebugWidget::clearButtonPressed, this, [this]() {
		qDebug()<<"CLEAR PRESSED FOR "<<objectName();
			if(!mNode.isNull()) {
				QSharedPointer<KeyStore> ks=mNode->keyStore();
				if(!ks.isNull()) {
					ks->clear();
				}
			}
		},OC_CONTYPE_NON_UNIQUE)) {
			qWarning()<<"ERROR: Could not connect";
		}
	}
}


void KeyStoreDebugWidget::onRealtimeChanged(bool realtime)
{
	OC_METHODGATE();
	Q_UNUSED(realtime);
}


void KeyStoreDebugWidget::onTimer()
{
	OC_METHODGATE();
	QMap<QString, QString> map;
	if(!mNode.isNull()) {
		QSharedPointer<KeyStore> ks=mNode->keyStore();
		if(!ks.isNull()) {
			map=ks->toMap();
		} else {
			map["Assoicate"]="null";
		}
	} else {
		map["Node"]="null";
	}
	setData(map);
}
