#ifndef PAIRINGLISTMODEL_HPP
#define PAIRINGLISTMODEL_HPP

#include "discovery/DiscoveryRole.hpp"

#include <QDebug>
#include <QAbstractListModel>

class PairingWizard;
class NodeAssociateStore;

class PairingListModel: public QAbstractListModel
{
	private:
		NodeAssociateStore &store;
		DiscoveryType typeFilter;
		PairingWizard &pwiz;


	private:
		bool filter(DiscoveryType &t) const;

	public:
		explicit PairingListModel(NodeAssociateStore &store, DiscoveryType typeFilter, PairingWizard &pwiz);
		virtual ~PairingListModel();

	public:
		int rowCount(const QModelIndex &) const;
		int columnCount(const QModelIndex &) const;
		QVariant data(const QModelIndex &index, int role) const;
		QVariant headerData(int section, Qt::Orientation orientation, int role) const;

};

#endif // PAIRINGLISTMODEL_HPP
