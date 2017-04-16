#ifndef TESTLISTMODEL_HPP
#define TESTLISTMODEL_HPP

#include <QAbstractListModel>

class TestListModel : public QAbstractListModel
{
		Q_OBJECT

	public:
		explicit TestListModel(QObject *parent = 0);

		// Header:
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

		bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

		// Basic functionality:
		int rowCount(const QModelIndex &parent = QModelIndex()) const override;

		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

		// Editable:
		bool setData(const QModelIndex &index, const QVariant &value,
					 int role = Qt::EditRole) override;

		Qt::ItemFlags flags(const QModelIndex& index) const override;

		// Add data:
		bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

		// Remove data:
		bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

	private:
};

#endif // TESTLISTMODEL_HPP