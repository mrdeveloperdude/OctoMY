#include "LocalIdentityDebugWidget.hpp"

#include "uptime/MethodGate.hpp"
#include "node/Node.hpp"

#include <QDebug>

LocalIdentityDebugWidget::LocalIdentityDebugWidget(QWidget *parent)
	: SimpleTableDebugWidget(parent)
	, mConfigureHelper("LocalIdentityDebugWidget", true, false, false, true, false)
{
	OC_METHODGATE();
}


LocalIdentityDebugWidget::~LocalIdentityDebugWidget()
{
	OC_METHODGATE();
}


void LocalIdentityDebugWidget::configure(QSharedPointer <Node> node)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		SimpleTableDebugWidget::configure("Local Identity");
		mNode=node;
	}
}


void LocalIdentityDebugWidget::onRealtimeChanged(bool realtime)
{
	OC_METHODGATE();
	Q_UNUSED(realtime);
}


void LocalIdentityDebugWidget::onTimer()
{
	OC_METHODGATE();
	QMap<QString, QString> map;
	if(!mNode.isNull()) {
		QSharedPointer<Associate> ass=mNode->nodeIdentity();
		if(!ass.isNull()) {
			map=ass->toMap();
		} else {
			map["Assoicate"]="null";
		}
	} else {
		map["Node"]="null";
	}
	setData(map);

}
