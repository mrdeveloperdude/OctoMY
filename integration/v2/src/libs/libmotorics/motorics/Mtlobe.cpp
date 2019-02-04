#include "Mtlobe.hpp"

#include "uptime/MethodGate.hpp"

#include <QVariantMap>
#include <QDebug>


Mtlobe::Mtlobe(QObject *parent)
	: QObject(parent)
{
	OC_METHODGATE();
}


QWidget *Mtlobe::configurationWidget()
{
	OC_METHODGATE();
	return nullptr;
}

QVariantMap Mtlobe::configuration()
{
	OC_METHODGATE();
	return QVariantMap();
}

void Mtlobe::setConfiguration(QVariantMap &)
{
	OC_METHODGATE();
	// Do nothing
	qDebug()<<"NO-OP";
}
