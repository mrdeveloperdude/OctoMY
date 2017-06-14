#ifndef PAIRINGLISTMODEL_HPP
#define PAIRINGLISTMODEL_HPP

#include "discovery/DiscoveryRole.hpp"

#include <QDebug>
#include <QAbstractListModel>

class PairingWizard;
class NodeAssociateStore;

class PairingListModel: public QAbstractListModel
{
	//Q_OBJECT
private:
	NodeAssociateStore &mStore;
	DiscoveryType mTypeFilter;
	PairingWizard &mPwiz;


private:
	bool filter(DiscoveryType &t) const;

public:
	explicit PairingListModel(NodeAssociateStore &mStore, DiscoveryType mTypeFilter, PairingWizard &mPwiz);
	virtual ~PairingListModel();

public:
	int rowCount(const QModelIndex &) const;
	int columnCount(const QModelIndex &) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;

	QString status();

public slots:

	void onPeersChanged();
};

#endif // PAIRINGLISTMODEL_HPP
