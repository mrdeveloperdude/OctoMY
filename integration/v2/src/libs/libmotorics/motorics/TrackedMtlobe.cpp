#include "TrackedMtlobe.hpp"

#include "TrackedMtlobeWidget.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"


#include <QVariantMap>
#include <QDebug>

TrackedMtlobe::TrackedMtlobe()
	: mConfigWidget(nullptr)
{
	OC_METHODGATE();

}

QWidget *TrackedMtlobe::configurationWidget()
{
	OC_METHODGATE();

	if(nullptr==mConfigWidget) {
		mConfigWidget=OC_NEW TrackedMtlobeWidget();
	}
	return mConfigWidget;
}

QVariantMap TrackedMtlobe::configuration()
{
	OC_METHODGATE();

	return QVariantMap();
}

void TrackedMtlobe::setConfiguration(QVariantMap &)
{
	OC_METHODGATE();

	// Do nothing
	qDebug()<<"NO-OP";
}
