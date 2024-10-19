#include "PairingListModel.hpp"

#include "address/Associate.hpp"
#include "node/Node.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"

#include <QDebug>
#include <QAbstractListModel>



PairingListModel::PairingListModel(QSharedPointer<AddressBook> store, NodeType typeFilter)
	: QAbstractListModel()
	, mStore(store)
	, mTypeFilter(typeFilter)
{
	OC_METHODGATE();
	if(!mStore.isNull()) {
		if(!connect(mStore.data(), &AddressBook::associatesChanged, this, &PairingListModel::onAssociateChanged, OC_CONTYPE)) {
			//if(!connect(&mStore, SIGNAL(peersChanged()), this, SLOT(onAssociateChanged()), OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect ";
		}
	} else {
		qWarning()<<"ERROR: No store";
	}
}


PairingListModel::~PairingListModel()
{
	OC_METHODGATE();
}


bool PairingListModel::filter(NodeType &t) const
{
	OC_METHODGATE();
	switch(mTypeFilter) {
	case(TYPE_AGENT):
		return ((TYPE_REMOTE==t) || (TYPE_HUB==t));
	case(TYPE_REMOTE):
		return ((TYPE_HUB==t) || (TYPE_AGENT==t));
	case(TYPE_HUB):
		return ((TYPE_AGENT==t) || (TYPE_REMOTE==t)||(TYPE_HUB==t)); //Hubs are onmnivorus

	//default:
	case(TYPE_ZOO):
	case(TYPE_UNKNOWN):
		return false;
	}
	return false;
}


void PairingListModel::onAssociateChanged()
{
	// qDebug()<<"FORWARDING UPDATE SIGNAL";
	if(!mStore.isNull()) {
		emit dataChanged(index(0, 0), index(mStore->associateCount(), 0));
	} else {
		qWarning()<<"ERROR: No store";
	}
}


int PairingListModel::rowCount(const QModelIndex &) const
{
	OC_METHODGATE();
	int ret=0;
	if(!mStore.isNull()) {
		QMap<QString, QSharedPointer<Associate> > participants=mStore->all();
		for(auto &p:participants) {
			if(nullptr != p) {
				auto t = p->type();
				if(filter(t)) {
					ret++;
				}
			}
		}
	} else {
		qWarning()<<"ERROR: No store";
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
	QSharedPointer<Associate> part;
	if(!mStore.isNull()) {
		QMap<QString, QSharedPointer<Associate> > participants=mStore->all();
		int targetRow = index.row();
		int rowCounter=0;
		for(auto &p:participants) {
			if(nullptr != p) {
				auto t = p->type();
				if(filter(t)) {
					if(rowCounter == targetRow) {
						part = p;
						break;
					}
					rowCounter++;
				}
			}
		}

	} else {
		qWarning()<<"ERROR: No store";
	}

	if (!part.isNull()) {
		NodeType t = part->type();
		if(filter(t)) {
			if(Qt::DisplayRole == role ) {
				return part->toVariantMap();
			} else if (Qt::ToolTipRole == role) {
				return nodeTypeToString(t) + ": " + part->id();
			}
		}
	}
	return QVariant();
}


QVariant PairingListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	OC_METHODGATE();
	Q_UNUSED(section);
	Q_UNUSED(orientation);
	Q_UNUSED(role);
	return QVariant();
}


QString PairingListModel::status()
{
	OC_METHODGATE();
	QString out("STATUS: ");
	if(!mStore.isNull()) {
		out += QString::number(mStore->associateCount());
	} else {
		out += "no";
	}
	out += " peers";
	return out;
}
