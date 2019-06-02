#include "LeggedMtlobe.hpp"

#include "LeggedMtlobeWidget.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QVariantMap>
#include <QDebug>

LeggedMtlobe::LeggedMtlobe()
	: mConfigWidget(nullptr)
{
	OC_METHODGATE();
}

QWidget *LeggedMtlobe::configurationWidget()
{
	OC_METHODGATE();
	if(nullptr==mConfigWidget) {
		mConfigWidget=OC_NEW LeggedMtlobeWidget();
	}
	return mConfigWidget;
}

QVariantMap LeggedMtlobe::configuration()
{
	OC_METHODGATE();
	return QVariantMap();
}

void LeggedMtlobe::setConfiguration(QVariantMap &)
{
	OC_METHODGATE();
	// Do nothing
	qDebug()<<"NO-OP";
}
