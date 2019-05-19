#include "KeyStoreDebugWidget.hpp"

#include "uptime/MethodGate.hpp"
#include "node/Node.hpp"

#include <QDebug>

KeyStoreDebugWidget::KeyStoreDebugWidget(QWidget *parent)
	: SimpleTableDebugWidget(parent)
	, mConfigureHelper("KeyStoreDebugWidget", true, false, false, true, false)
{
	OC_METHODGATE();
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
