#include "Mtlobe.hpp"


#include <QVariantMap>
#include <QDebug>


Mtlobe::Mtlobe(QObject *parent) : QObject(parent)
{

}


QWidget *Mtlobe::configurationWidget()
{
	return nullptr;
}

QVariantMap Mtlobe::configuration()
{
	return QVariantMap();
}

void Mtlobe::setConfiguration(QVariantMap &)
{
	// Do nothing
	qDebug()<<"NO-OP";
}
