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


void LocalIdentityDebugWidget::setData(QMap<QString, QString> data)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		SimpleTableDebugWidget::setData(data);
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
	QString assString;
	QMap<QString, QString> map;
	if(!mNode.isNull()) {
		QSharedPointer<Associate> ass=mNode->nodeIdentity();
		if(!ass.isNull()) {
			assString=ass->toString();
			if(assString!=mLastAssString) {
				map=ass->toMap();
			}
		} else {
			assString="ass-null";
			map["Assoicate"]="null";
		}
	} else {
		assString="node-null";
		map["Node"]="null";
	}
	if(assString!=mLastAssString) {
		mLastAssString=assString;
		setData(map);
	}
}
