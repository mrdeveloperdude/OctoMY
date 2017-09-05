#include "HoveringMtlobe.hpp"

#include "HoveringMtlobeWidget.hpp"


#include <QVariantMap>
#include <QDebug>

HoveringMtlobe::HoveringMtlobe()
	: mConfigWidget(nullptr)
{

}

QWidget *HoveringMtlobe::configurationWidget()
{
	if(nullptr==mConfigWidget) {
		mConfigWidget=OC_NEW HoveringMtlobeWidget();
	}
	return mConfigWidget;
}

QVariantMap HoveringMtlobe::configuration()
{
	return QVariantMap();
}

void HoveringMtlobe::setConfiguration(QVariantMap &)
{
	// Do nothing
	qDebug()<<"NO-OP";
}
