#include "TriggerListModel.hpp"

// TODO: This turned out to not be very useful. Remove it if it still is the case when you read this after 2017!

TriggerListModel::TriggerListModel(QObject *parent)
	: QAbstractListModel(parent)
{
}

int TriggerListModel::rowCount(const QModelIndex &parent) const
{
	// For list models only the root node (an invalid parent) should return the list's size. For all
	// other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
	if (parent.isValid()) {
		return 0;
	}
	return mTriggers.size();
}

QVariant TriggerListModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid()) {
		return QVariant();
	}
	return mTriggers[index.row()]->toVariant();
}

bool TriggerListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (data(index, role) != value) {
		// FIXME: Implement me!
		emit dataChanged(index, index, QVector<int>() << role);
		return true;
	}
	return false;
}

Qt::ItemFlags TriggerListModel::flags(const QModelIndex &index) const
{
	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}

	return Qt::ItemIsEditable; // FIXME: Implement me!
}

bool TriggerListModel::insertRows(int row, int count, const QModelIndex &parent)
{
	beginInsertRows(parent, row, row + count - 1);
	for(int i=0;i<count;++i){
		mTriggers<<new Trigger();
	}
	endInsertRows();
	return false;
}

bool TriggerListModel::removeRows(int row, int count, const QModelIndex &parent)
{
	beginRemoveRows(parent, row, row + count - 1);
	// FIXME: Implement me!
	endRemoveRows();
	return false;
}
