#include "LogModel.hpp"

#include "uptime/MethodGate.hpp"

LogModel::LogModel(QSharedPointer<LogStorage> storage, QObject *parent)
	: QAbstractListModel(parent), m_logStorage(storage)
{
	OC_METHODGATE();
	updateLogs();
}

void LogModel::setStorage(QSharedPointer<LogStorage> storage){
	OC_METHODGATE();
	m_logStorage = storage;
	updateLogs();
}

int LogModel::rowCount(const QModelIndex &) const
{
	OC_METHODGATE();
	return m_logs.size();
}

QVariant LogModel::data(const QModelIndex &index, int role) const
{
	OC_METHODGATE();
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
	OC_METHODGATE();
	beginResetModel();
	m_logs = m_logStorage->readLogs(500);
	endResetModel();
}

void LogModel::setFilter(const QString &filter)
{
	OC_METHODGATE();
	m_filter = filter;
	updateLogs();
}
