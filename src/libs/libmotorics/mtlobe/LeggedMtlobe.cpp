#include "LeggedMtlobe.hpp"

#include "LeggedMtlobeWidget.hpp"


#include <QVariantMap>
#include <QDebug>

LeggedMtlobe::LeggedMtlobe()
	: mConfigWidget(nullptr)
{

}

QWidget *LeggedMtlobe::configurationWidget()
{
	if(nullptr==mConfigWidget) {
		mConfigWidget=new LeggedMtlobeWidget();
	}
	return mConfigWidget;
}

QVariantMap LeggedMtlobe::configuration()
{
	return QVariantMap();
}

void LeggedMtlobe::setConfiguration(QVariantMap &)
{
	// Do nothing
	qDebug()<<"NO-OP";
}
