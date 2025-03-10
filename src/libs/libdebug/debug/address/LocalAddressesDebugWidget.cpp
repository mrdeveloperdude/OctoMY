#include "LocalAddressesDebugWidget.hpp"

#include "uptime/MethodGate.hpp"
#include "node/Node.hpp"

#include <QDebug>

LocalAddressesDebugWidget::LocalAddressesDebugWidget(QWidget *parent)
	: SimpleTableDebugWidget(parent)
	, mConfigureHelper("LocalAddressesDebugWidget", true, false, false, true, false)
{
	OC_METHODGATE();
}


LocalAddressesDebugWidget::~LocalAddressesDebugWidget()
{
	OC_METHODGATE();
}


void LocalAddressesDebugWidget::configure(QSharedPointer <Node> node)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		SimpleTableDebugWidget::configure("Local Addresses");
		mNode=node;
	}
}


void LocalAddressesDebugWidget::onRealtimeChanged(bool realtime)
{
	OC_METHODGATE();
	Q_UNUSED(realtime);
}


void LocalAddressesDebugWidget::onTimer()
{
	OC_METHODGATE();
	QMap<QString, QString> map;
	if(!mNode.isNull()) {
		QSharedPointer<LocalAddressList> lal=mNode->localAddressList();
		if(!lal.isNull()) {
			map=lal->toMap();
		} else {
			map["Local Address List"]="null";
		}
	} else {
		map["Node"]="null";
	}
	setData(map);
}
