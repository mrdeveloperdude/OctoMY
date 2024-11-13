#include "WheeledLobe.hpp"

#include "WheeledLobeWidget.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QVariantMap>
#include <QDebug>


WheeledLobe::WheeledLobe(QObject *parent)
	: Lobe("Wheeled", "Locomotion by 1 or more wheels", ":/icons/wheels.svg", parent)
	, mConfigWidget(nullptr)
{
	OC_METHODGATE();

}


QWidget *WheeledLobe::configurationWidget()
{
	OC_METHODGATE();
	if(nullptr==mConfigWidget) {
		mConfigWidget=OC_NEW WheeledLobeWidget();
	}
	return mConfigWidget;
}

QVariantMap WheeledLobe::configuration()
{
	OC_METHODGATE();
	return QVariantMap();
}

void WheeledLobe::setConfiguration(QVariantMap &)
{
	OC_METHODGATE();
	// Do nothing
	qDebug()<<"NO-OP";
}
