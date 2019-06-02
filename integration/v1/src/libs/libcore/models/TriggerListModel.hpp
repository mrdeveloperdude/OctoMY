#ifndef TRIGGERLISTMODEL_HPP
#define TRIGGERLISTMODEL_HPP


#include "trigger/Trigger.hpp"

#include <QAbstractListModel>

// TODO: This turned out to not be very useful. Remove it if it still is the case when you read this after 2017!


class TriggerListModel : public QAbstractListModel
{

	Q_OBJECT
private:

	QList<Trigger *> mTriggers;
public:
	explicit TriggerListModel(QObject *parent = 0);

public:

	void add(Trigger *t)
	{
		if(nullptr!=t) {
			mTriggers<<t;
			insertRows(0,1);
		}
	}

public:

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
};

#endif // TRIGGERLISTMODEL_HPP
