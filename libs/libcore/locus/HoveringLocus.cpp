#include "HoveringLocus.hpp"

#include "HoveringLocusWidget.hpp"


#include <QVariantMap>
#include <QDebug>

HoveringLocus::HoveringLocus()
	: mConfigWidget(nullptr)
{

}

QWidget *HoveringLocus::configurationWidget()
{
	if(nullptr==mConfigWidget) {
		mConfigWidget=new HoveringLocusWidget();
	}
	return mConfigWidget;
}

QVariantMap HoveringLocus::configuration()
{
	return QVariantMap();
}

void HoveringLocus::setConfiguration(QVariantMap &)
{
	// Do nothing
	qDebug()<<"NO-OP";
}
