#include "TrackedMtlobe.hpp"

#include "TrackedMtlobeWidget.hpp"


#include <QVariantMap>
#include <QDebug>

TrackedMtlobe::TrackedMtlobe()
	: mConfigWidget(nullptr)
{

}

QWidget *TrackedMtlobe::configurationWidget()
{
	if(nullptr==mConfigWidget) {
		mConfigWidget=new TrackedMtlobeWidget();
	}
	return mConfigWidget;
}

QVariantMap TrackedMtlobe::configuration()
{
	return QVariantMap();
}

void TrackedMtlobe::setConfiguration(QVariantMap &)
{
	// Do nothing
	qDebug()<<"NO-OP";
}
