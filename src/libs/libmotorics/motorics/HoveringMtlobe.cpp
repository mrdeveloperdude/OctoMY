#include "HoveringMtlobe.hpp"

#include "HoveringMtlobeWidget.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QVariantMap>
#include <QDebug>

HoveringMtlobe::HoveringMtlobe()
	: mConfigWidget(nullptr)
{
	OC_METHODGATE();
}

QWidget *HoveringMtlobe::configurationWidget()
{
	OC_METHODGATE();
	if(nullptr==mConfigWidget) {
		mConfigWidget=OC_NEW HoveringMtlobeWidget();
	}
	return mConfigWidget;
}

QVariantMap HoveringMtlobe::configuration()
{
	OC_METHODGATE();
	return QVariantMap();
}

void HoveringMtlobe::setConfiguration(QVariantMap &)
{
	OC_METHODGATE();
	// Do nothing
	qDebug()<<"NO-OP";
}
