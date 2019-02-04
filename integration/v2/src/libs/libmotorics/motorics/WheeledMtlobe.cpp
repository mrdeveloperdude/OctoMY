#include "WheeledMtlobe.hpp"

#include "WheeledMtlobeWidget.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QVariantMap>
#include <QDebug>


WheeledMtlobe::WheeledMtlobe()
	: mConfigWidget(nullptr)
{
	OC_METHODGATE();

}


QWidget *WheeledMtlobe::configurationWidget()
{
	OC_METHODGATE();
	if(nullptr==mConfigWidget) {
		mConfigWidget=OC_NEW WheeledMtlobeWidget();
	}
	return mConfigWidget;
}

QVariantMap WheeledMtlobe::configuration()
{
	OC_METHODGATE();
	return QVariantMap();
}

void WheeledMtlobe::setConfiguration(QVariantMap &)
{
	OC_METHODGATE();
	// Do nothing
	qDebug()<<"NO-OP";
}
