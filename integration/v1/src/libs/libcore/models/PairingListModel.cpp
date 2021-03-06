#include "PairingListModel.hpp"

#include "../widgets/PairingWizard.hpp"
#include "basic/Settings.hpp"
#include "node/Node.hpp"
#include "identity/Identicon.hpp"
#include "basic/Associate.hpp"
#include "security/PortableID.hpp"
#include "utility/Standard.hpp"

#include <QDebug>
#include <QAbstractListModel>

bool PairingListModel::filter(NodeType &t) const
{
	OC_METHODGATE();
	switch(mTypeFilter) {
	case(TYPE_AGENT):
		return ((TYPE_REMOTE==t)||(TYPE_HUB==t));
	case(TYPE_REMOTE):
		return ((TYPE_HUB==t)||(TYPE_AGENT==t));
	case(TYPE_HUB):
		return ((TYPE_AGENT==t)||(TYPE_REMOTE==t)||(TYPE_HUB==t)); //Hubs are onmnivorus
	default:
		return false;
	}
	return false;
}


PairingListModel::PairingListModel(AddressBook &store, NodeType typeFilter, PairingWizard &pwiz)
	: QAbstractListModel(&pwiz)
	, mStore(store)
	, mTypeFilter(typeFilter)
	, mPwiz(pwiz)
{
	OC_METHODGATE();
	if(!connect(&mStore, &AddressBook::associatesChanged, this, &PairingListModel::onAssociateChanged, OC_CONTYPE)) {
		//if(!connect(&mStore, SIGNAL(peersChanged()), this, SLOT(onAssociateChanged()), OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect ";
	}
}

PairingListModel::~PairingListModel()
{
	OC_METHODGATE();
}

void PairingListModel::onAssociateChanged()
{
	qDebug()<<"FORWARDING UPDATE SIGNAL";
	emit dataChanged(index(0,0),index(mStore.associateCount(),0));
}

int PairingListModel::rowCount(const QModelIndex &) const
{
	OC_METHODGATE();
	int ret=0;
	QMap<QString, QSharedPointer<Associate> > participants=mStore.all();
	for(QMap<QString, QSharedPointer<Associate> >::const_iterator it=participants.begin(), e=participants.end(); it!=e; ++it) {
		QSharedPointer<Associate> p=it.value();
		if(nullptr!=p) {
			NodeType t=p->type();
			if(filter(t)) {
				ret++;
			}
		}
	}
	//qDebug()<<"PairingList Rowcount: "<<ret;
	return ret;
}

int PairingListModel::columnCount(const QModelIndex &) const
{
	OC_METHODGATE();
	return 1;
}

QVariant PairingListModel::data(const QModelIndex &index, int role) const
{
	OC_METHODGATE();
	QMap<QString, QSharedPointer<Associate> > &participants=mStore.all();
	int targetRow=index.row();
	int rowCounter=0;
	QSharedPointer<Associate> part;

	for(QMap<QString, QSharedPointer<Associate> >::const_iterator it=participants.begin(), e=participants.end(); it!=e; ++it) {
		QSharedPointer<Associate> p=it.value();
		if(nullptr!=p) {
			NodeType t=p->type();
			if(filter(t)) {
				if(rowCounter==targetRow) {
					part=p;
					break;
				}
				rowCounter++;
			}
		}
	}

	if (nullptr!=part) {
		NodeType t=part->type();
		if(filter(t)) {
			if(Qt::DisplayRole == role ) {
				return part->toVariantMap();
			} else if (Qt::ToolTipRole == role) {
				return nodeTypeToString(t) +": " +part->id();
			}
		}
	}
	return QVariant();
}


QVariant PairingListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	OC_METHODGATE();
	return QVariant();
}


QString PairingListModel::status()
{
	OC_METHODGATE();
	QString out("STATUS: ");
	out+=QString::number(mStore.associateCount());
	out+=" peers";
	return out;
}
