#include "WheeledLocus.hpp"

#include "WheeledLocusWidget.hpp"

#include <QVariantMap>
#include <QDebug>


WheeledLocus::WheeledLocus()
	: mConfigWidget(nullptr)
{

}




QWidget *WheeledLocus::configurationWidget()
{
	if(nullptr==mConfigWidget) {
		mConfigWidget=new WheeledLocusWidget();
	}
	return mConfigWidget;
}

QVariantMap WheeledLocus::configuration()
{
	return QVariantMap();
}

void WheeledLocus::setConfiguration(QVariantMap &)
{
	// Do nothing
	qDebug()<<"NO-OP";
}
