#include "Locus.hpp"


#include <QVariantMap>
#include <QDebug>


Locus::Locus(QObject *parent) : QObject(parent)
{

}


QWidget *Locus::configurationWidget()
{
	return nullptr;
}

QVariantMap Locus::configuration()
{
	return QVariantMap();
}

void Locus::setConfiguration(QVariantMap &)
{
	// Do nothing
	qDebug()<<"NO-OP";
}
