#include "KeyStoreDebugWidget.hpp"

#include "uptime/MethodGate.hpp"
#include "node/Node.hpp"

#include <QDebug>

KeyStoreDebugWidget::KeyStoreDebugWidget(QWidget *parent)
	: SimpleTableDebugWidget(parent)
	, mConfigureHelper("LocalIdentityDebugWidget", true, false, false, true, false)
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
		SimpleTableDebugWidget::configure("Local Identity");
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
	QString ksString;
	QMap<QString, QString> map;
	if(!mNode.isNull()) {
		QSharedPointer<KeyStore> ks=mNode->keyStore();
		if(!ks.isNull()) {
			ksString=ks->toString();
			if(ksString!=mLastKsString) {
				map=ks->toMap();
			}
		} else {
			ksString="ks-null";
			map["Assoicate"]="null";
		}
	} else {
		ksString="node-null";
		map["Node"]="null";
	}
	if(ksString!=mLastKsString) {
		mLastKsString=ksString;
		setData(map);
	}
}
