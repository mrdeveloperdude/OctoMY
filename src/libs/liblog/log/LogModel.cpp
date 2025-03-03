#include "LogModel.hpp"

LogModel::LogModel(QSharedPointer<LogStorage> storage, QObject *parent)
	: QAbstractListModel(parent), m_logStorage(storage)
{
	updateLogs();
}

int LogModel::rowCount(const QModelIndex &) const
{
	return m_logs.size();
}

QVariant LogModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() || index.row() >= m_logs.size()){
		return QVariant();
	}
	
	if (role == Qt::DisplayRole){
		return m_logs.at(index.row());
	}
	
	return QVariant();
}

void LogModel::updateLogs()
{
	beginResetModel();
	m_logs = m_logStorage->readLogs(500);
	endResetModel();
}

void LogModel::setFilter(const QString &filter)
{
	m_filter = filter;
	updateLogs();
}
