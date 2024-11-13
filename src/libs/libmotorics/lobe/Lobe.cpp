#include "Lobe.hpp"

#include "uptime/MethodGate.hpp"

#include <QVariantMap>
#include <QDebug>


Lobe::Lobe(const QString &title, const QString &description, const QString &icon, QObject *parent)
	: QObject(parent)
	, mTitle(title)
	, mDescription(description)
	, mIcon(icon)
{
	OC_METHODGATE();
}



// Lobe interface
//////////////////////////////////////////////


QWidget *Lobe::configurationWidget()
{
	OC_METHODGATE();
	return nullptr;
}

QVariantMap Lobe::configuration()
{
	OC_METHODGATE();
	return QVariantMap();
}

void Lobe::setConfiguration(QVariantMap &)
{
	OC_METHODGATE();
	// Do nothing
	qDebug()<<"NO-OP";
}


// Lobe wrapping
//////////////////////////////////////////////


QString Lobe::lobeTitle() const
{
	OC_METHODGATE();
	return mTitle;
}

QString Lobe::lobeDescription() const
{
	OC_METHODGATE();
	return mDescription;
}

QString Lobe::lobeIcon() const
{
	OC_METHODGATE();
	return mIcon;
}
