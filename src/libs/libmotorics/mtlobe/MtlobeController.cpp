#include "MtlobeController.hpp"

#include <QDebug>

MtlobeController::MtlobeController(QObject *parent)
	: QObject(parent)
{

}

Mtlobe *MtlobeController::addLobe(QString type)
{
	Mtlobe *l=mFactory.mtlobeFactory(type);
	if(nullptr!=l) {
		mLoci.push_back(l);
	}
	return l;
}

MtlobeSet &MtlobeController::lobes()
{
	return mLoci;
}


void MtlobeController::onMtlobeWidgetDeleted(quint32 id)
{
	qDebug()<<"DELETED: "<<id;
}
