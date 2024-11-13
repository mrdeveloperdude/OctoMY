#include "TrackedLobe.hpp"

#include "TrackedLobeWidget.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"


#include <QVariantMap>
#include <QDebug>

TrackedLobe::TrackedLobe(QObject *parent)
	: Lobe("Tracked", "Locomotion by 1 or more tracks", ":/icons/threads.svg", parent)
	, mConfigWidget(nullptr)
{
	OC_METHODGATE();

}

QWidget *TrackedLobe::configurationWidget()
{
	OC_METHODGATE();

	if(nullptr==mConfigWidget) {
		mConfigWidget=OC_NEW TrackedLobeWidget();
	}
	return mConfigWidget;
}

QVariantMap TrackedLobe::configuration()
{
	OC_METHODGATE();

	return QVariantMap();
}

void TrackedLobe::setConfiguration(QVariantMap &)
{
	OC_METHODGATE();

	// Do nothing
	qDebug()<<"NO-OP";
}
