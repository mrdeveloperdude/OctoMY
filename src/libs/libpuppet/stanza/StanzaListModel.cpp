#include "StanzaListModel.hpp"

#include "stanza/Stanza.hpp"
#include "stanza/StanzaBook.hpp"
#include "uptime/MethodGate.hpp"

#include <QDebug>
#include <QAbstractListModel>



StanzaListModel::StanzaListModel(QSharedPointer<StanzaBook> stanzaBook)
	: QAbstractListModel()
	, mStore(stanzaBook)
{
	OC_METHODGATE();
}


StanzaListModel::~StanzaListModel()
{
	OC_METHODGATE();
}


void StanzaListModel::onStanzaChanged()
{
	// qDebug()<<"FORWARDING UPDATE SIGNAL";
	if(!mStore.isNull()) {
		emit dataChanged(index(0, 0), index(mStore->stanzaCount(), 0));
	} else {
		qWarning()<<"ERROR: No store";
	}
}


int StanzaListModel::rowCount(const QModelIndex &) const
{
	OC_METHODGATE();
	int ret = 0;
	if(!mStore.isNull()) {
		ret = mStore->stanzaCount();
	} else {
		qWarning()<<"ERROR: No store";
	}
	// qDebug()<<"StanzaListModel Rowcount: "<<ret;
	return ret;
}


int StanzaListModel::columnCount(const QModelIndex &) const
{
	OC_METHODGATE();
	return 1;
}


QVariant StanzaListModel::data(const QModelIndex &index, int role) const
{
	OC_METHODGATE();
	QSharedPointer<Stanza> part;
	if(!mStore.isNull()) {
		QMap<QString, QSharedPointer<Stanza> > stanzas=mStore->all();
		int targetRow = index.row();
		int rowCounter=0;
		for(auto &p:stanzas) {
			if(nullptr != p) {
				auto t = p->id();
				if(rowCounter == targetRow) {
					part = p;
					break;
				}
				rowCounter++;
			}
		}
	} else {
		qWarning()<<"ERROR: No store";
	}

	if (!part.isNull()) {
		auto t = part->id();
		if(Qt::DisplayRole == role ) {
			return part->toVariantMap();
		} else if (Qt::ToolTipRole == role) {
			return part->id();
		}
	} else {
		qWarning()<<"ERROR: No part";
	}
	return QVariant();
}


QVariant StanzaListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	OC_METHODGATE();
	Q_UNUSED(section);
	Q_UNUSED(orientation);
	Q_UNUSED(role);
	return QVariant();
}


QString StanzaListModel::status()
{
	OC_METHODGATE();
	QString out("STATUS: ");
	if(!mStore.isNull()) {
		out += QString::number(mStore->stanzaCount());
	} else {
		out += "no";
	}
	out += " stanzas";
	return out;
}


void StanzaListModel::invalidate(){
	beginResetModel();
	endResetModel();
}
