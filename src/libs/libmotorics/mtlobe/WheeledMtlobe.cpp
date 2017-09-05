#include "WheeledMtlobe.hpp"

#include "WheeledMtlobeWidget.hpp"

#include <QVariantMap>
#include <QDebug>


WheeledMtlobe::WheeledMtlobe()
	: mConfigWidget(nullptr)
{

}


QWidget *WheeledMtlobe::configurationWidget()
{
	if(nullptr==mConfigWidget) {
		mConfigWidget=new WheeledMtlobeWidget();
	}
	return mConfigWidget;
}

QVariantMap WheeledMtlobe::configuration()
{
	return QVariantMap();
}

void WheeledMtlobe::setConfiguration(QVariantMap &)
{
	// Do nothing
	qDebug()<<"NO-OP";
}
