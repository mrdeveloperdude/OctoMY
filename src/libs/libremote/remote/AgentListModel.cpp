#include "AgentListModel.hpp"


#include "address/Associate.hpp"
#include "discovery/AddressBook.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"


AgentListModel::AgentListModel(QSharedPointer<AddressBook> store)
	: QAbstractListModel()
	, mStore(store)
{
	OC_METHODGATE();
	if(!mStore.isNull()) {
		if(!connect(mStore.data(), &AddressBook::associatesChanged, this, &AgentListModel::onAssociateChanged, OC_CONTYPE)) {
			//if(!connect(&mStore, SIGNAL(peersChanged()), this, SLOT(onAssociateChanged()), OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect ";
		}
	} else {
		qWarning()<<"ERROR: No store";
	}
}


AgentListModel::~AgentListModel()
{
	OC_METHODGATE();
}

void AgentListModel::onAssociateChanged()
{
	OC_METHODGATE();
	// qDebug()<<"FORWARDING UPDATE SIGNAL";
	if(!mStore.isNull()) {
		emit dataChanged(index(0, 0), index(mStore->associateCount(), 0));
	} else {
		qWarning()<<"ERROR: No store";
	}
}


int AgentListModel::rowCount(const QModelIndex &) const
{
	OC_METHODGATE();
	int ret=0;
	if(!mStore.isNull()) {
		QVector<QueryRule> f;
		f.append(QueryRule(TYPE_AGENT, false, true, true));
		auto participants = mStore->filter(f);
		ret = participants.size();
	} else {
		qWarning()<<"ERROR: No store";
	}
	return ret;
}


int AgentListModel::columnCount(const QModelIndex &) const
{
	OC_METHODGATE();
	return 1;
}


QVariant AgentListModel::data(const QModelIndex &index, int role) const
{
	OC_METHODGATE();
	QVariant ret;
	const auto row = index.row();
	if(row < 0){
		return ret;
	}
	if(!mStore.isNull()) {
		QVector<QueryRule> f;
		f.append(QueryRule(TYPE_AGENT, false, true, true));
		auto participants = mStore->filter(f);
		if(row >= participants.size()){
			return ret;
		}
		int rowCounter = 0;
		for(auto &p:participants) {
			if(nullptr != p) {
				if(rowCounter == row) {
					ret = p->toVariantMap();
					break;
				}
				rowCounter++;
			}
		}
	} else {
		qWarning()<<"ERROR: No store";
	}
	return ret;
}


QVariant AgentListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	OC_METHODGATE();
	Q_UNUSED(section);
	Q_UNUSED(orientation);
	Q_UNUSED(role);
	return QVariant();
}


QString AgentListModel::status()
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


void AgentListModel::invalidate(){
	OC_METHODGATE();
	beginResetModel();
	endResetModel();
}
