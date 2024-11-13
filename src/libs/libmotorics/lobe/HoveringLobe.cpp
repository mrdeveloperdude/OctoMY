#include "HoveringLobe.hpp"

#include "HoveringLobeWidget.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QVariantMap>
#include <QDebug>

HoveringLobe::HoveringLobe(QObject *parent)
	: Lobe("Hovering", "Locomotion by hovering", ":/icons/rocket.svg", parent)
	, mConfigWidget(nullptr)
{
	OC_METHODGATE();
}

QWidget *HoveringLobe::configurationWidget()
{
	OC_METHODGATE();
	if(nullptr==mConfigWidget) {
		mConfigWidget=OC_NEW HoveringLobeWidget();
	}
	return mConfigWidget;
}

QVariantMap HoveringLobe::configuration()
{
	OC_METHODGATE();
	return QVariantMap();
}

void HoveringLobe::setConfiguration(QVariantMap &)
{
	OC_METHODGATE();
	// Do nothing
	qDebug()<<"NO-OP";
}
