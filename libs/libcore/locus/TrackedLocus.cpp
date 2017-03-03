#include "TrackedLocus.hpp"

#include "TrackedLocusWidget.hpp"


#include <QVariantMap>
#include <QDebug>

TrackedLocus::TrackedLocus()
	: mConfigWidget(nullptr)
{

}

QWidget *TrackedLocus::configurationWidget()
{
	if(nullptr==mConfigWidget) {
		mConfigWidget=new TrackedLocusWidget();
	}
	return mConfigWidget;
}

QVariantMap TrackedLocus::configuration()
{
	return QVariantMap();
}

void TrackedLocus::setConfiguration(QVariantMap &)
{
	// Do nothing
	qDebug()<<"NO-OP";
}
