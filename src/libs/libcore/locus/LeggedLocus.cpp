#include "LeggedLocus.hpp"

#include "LeggedLocusWidget.hpp"


#include <QVariantMap>
#include <QDebug>

LeggedLocus::LeggedLocus()
	: mConfigWidget(nullptr)
{

}

QWidget *LeggedLocus::configurationWidget()
{
	if(nullptr==mConfigWidget) {
		mConfigWidget=new LeggedLocusWidget();
	}
	return mConfigWidget;
}

QVariantMap LeggedLocus::configuration()
{
	return QVariantMap();
}

void LeggedLocus::setConfiguration(QVariantMap &)
{
	// Do nothing
	qDebug()<<"NO-OP";
}
