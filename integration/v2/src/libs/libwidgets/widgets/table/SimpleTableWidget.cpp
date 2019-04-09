#include "SimpleTableWidget.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/Assert.hpp"

#include "utility/data/Data.hpp"

#include <QHeaderView>

class SimpleTableModel : public QAbstractTableModel
{

	QMap<QString, QString> mOriginalData;
	QVector<QPair<QString, QString> > mData;

public:

	SimpleTableModel(QWidget *parent = nullptr):
		QAbstractTableModel(parent)
	{
		OC_METHODGATE();
	}

	int rowCount(const QModelIndex &) const
	{
		OC_METHODGATE();
		return mData.count();
	}

	int columnCount(const QModelIndex &) const
	{
		OC_METHODGATE();
		return 2;
	}

	QVariant data(const QModelIndex &index, int role) const
	{
		OC_METHODGATE();
		if (role != Qt::DisplayRole && role != Qt::EditRole) {
			return QVariant();
		}
		QPair<QString, QString>  entry = mData[index.row()];
		//mData.at(index.row());
		switch (index.column()) {
		case 0:
			return entry.first;
		case 1 :
			return entry.second;
		};
		return QVariant();
	}

	QVariant headerData(int section, Qt::Orientation orientation, int role) const
	{
		OC_METHODGATE();
		if (orientation != Qt::Horizontal) {
			return QVariant();
		}
		if (role != Qt::DisplayRole) {
			return QVariant();
		}
		switch (section) {
		case 0:
			return "Name";
		case 1:
			return "Value";

		}
		return QVariant();
	}

	void setTableData(QMap<QString, QString> &b)
	{
		OC_METHODGATE();
		beginResetModel();
		endResetModel();
		beginInsertRows(QModelIndex(), 0, b.count());
		mOriginalData=b;
		mData.clear();
		int index=0;
		mData=utility::data::convert(mOriginalData);
		for (auto it = mOriginalData.constBegin(), end = mOriginalData.constEnd(); it != end; ++it) {
			mData[index++]=QPair<QString, QString>(it.key(), it.value());
		}
		endInsertRows();
	}

};



SimpleTableWidget::SimpleTableWidget(QWidget *parent)
	: QTableView(parent)
	, model(new SimpleTableModel(this))
{
	OC_METHODGATE();
	OC_ASSERT(nullptr != model);
	setModel(model);
	horizontalHeader()->setStretchLastSection(true);
	horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
			//setResizeMode(QHeaderView::Stretch);
}


void SimpleTableWidget::setData(QMap<QString, QString> &b)
{
	OC_METHODGATE();
	if(nullptr != model) {
		model->setTableData(b);
		resizeColumnsToContents();
	}
}

