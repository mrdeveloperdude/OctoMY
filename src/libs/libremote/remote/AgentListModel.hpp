#ifndef AGENTLISTMODEL_HPP
#define AGENTLISTMODEL_HPP

#include <QAbstractListModel>

#include "uptime/SharedPointerWrapper.hpp"

class AddressBook;

class AgentListModel : public QAbstractListModel
{
	Q_OBJECT
private:
	QSharedPointer<AddressBook> mStore;

public:
	explicit AgentListModel(QSharedPointer<AddressBook> store);
	virtual ~AgentListModel();
	
	// QAbstractListModel interface
public:
	int rowCount(const QModelIndex &) const;
	int columnCount(const QModelIndex &) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;

public:
	QString status();
	void invalidate();

public slots:
	void onAssociateChanged();

};

#endif // AGENTLISTMODEL_HPP
