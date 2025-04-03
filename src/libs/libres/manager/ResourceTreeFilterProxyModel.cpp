#include "ResourceTreeFilterProxyModel.hpp"


ResourceTreeFilterProxyModel::ResourceTreeFilterProxyModel(QObject *parent)
	: QSortFilterProxyModel(parent)
{
	setRecursiveFilteringEnabled(true);
}


bool ResourceTreeFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
	QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
	if (!index.isValid()){
		return false;
	}
	QString filterText = filterRegularExpression().pattern();
	if (sourceModel()->data(index, Qt::DisplayRole).toString().contains(filterText, Qt::CaseInsensitive)){
		return true;
	}
	const int childCount = sourceModel()->rowCount(index);
	for (int i = 0; i < childCount; ++i) {
		if (filterAcceptsRow(i, index)){
			return true;
		}
	}
	return false;
}
