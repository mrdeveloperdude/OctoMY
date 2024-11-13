#include "LeggedLobe.hpp"

#include "LeggedLobeWidget.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QVariantMap>
#include <QDebug>

LeggedLobe::LeggedLobe(QObject *parent)
	: Lobe("Legged", "Locomotion by 1 or more legs", ":/icons/hexapod.svg", parent)
	, mConfigWidget(nullptr)
{
	OC_METHODGATE();
}

QWidget *LeggedLobe::configurationWidget()
{
	OC_METHODGATE();
	if(nullptr==mConfigWidget) {
		mConfigWidget=OC_NEW LeggedLobeWidget();
	}
	return mConfigWidget;
}

QVariantMap LeggedLobe::configuration()
{
	OC_METHODGATE();
	return QVariantMap();
}

void LeggedLobe::setConfiguration(QVariantMap &)
{
	OC_METHODGATE();
	// Do nothing
	qDebug()<<"NO-OP";
}
