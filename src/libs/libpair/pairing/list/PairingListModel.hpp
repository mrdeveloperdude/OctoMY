#ifndef PAIRINGLISTMODEL_H
#define PAIRINGLISTMODEL_H

#include "node/NodeType.hpp"

#include <QDebug>
#include <QAbstractListModel>

class AddressBook;

class PairingListModel: public QAbstractListModel
{
	//Q_OBJECT
private:
	QSharedPointer<AddressBook> mStore;
	NodeType mTypeFilter;

public:
	explicit PairingListModel(QSharedPointer<AddressBook> addressBook, NodeType type);
	virtual ~PairingListModel();

private:
	bool filter(NodeType &t) const;

	// QAbstractListModel interface
public:
	int rowCount(const QModelIndex &) const;
	int columnCount(const QModelIndex &) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;

public:
	QString status();

public slots:
	void onAssociateChanged();

};

#endif
// PAIRINGLISTMODEL_HPP
