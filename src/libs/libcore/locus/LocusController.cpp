#include "LocusController.hpp"

#include <QDebug>

LocusController::LocusController(QObject *parent)
	: QObject(parent)
{

}

Locus *LocusController::addLocus(QString type)
{
	Locus *l=mFactory.locusFactory(type);
	if(nullptr!=l) {
		mLoci.push_back(l);
	}
	return l;
}

LocusSet &LocusController::loci()
{
	return mLoci;
}


void LocusController::onLocusWidgetDeleted(quint32 id)
{
	qDebug()<<"DELETED: "<<id;
}
