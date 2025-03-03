#ifndef LOGMODEL_HPP
#define LOGMODEL_HPP

#include <QAbstractListModel>

#include "LogStorage.hpp"

class LogModel : public QAbstractListModel {
	Q_OBJECT
private:
	QSharedPointer<LogStorage> m_logStorage;
	QStringList m_logs;
	QString m_filter;
	
public:
	explicit LogModel(QSharedPointer<LogStorage> storage, QObject *parent = nullptr);
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	void updateLogs();
	void setFilter(const QString &filter);
};

#endif // LOGMODEL_HPP
