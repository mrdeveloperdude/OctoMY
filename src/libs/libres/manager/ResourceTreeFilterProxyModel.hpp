#ifndef RESOURCETREEFILTERPROXYMODEL_HPP
#define RESOURCETREEFILTERPROXYMODEL_HPP

#include <QSortFilterProxyModel>

class ResourceTreeFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT
public:
	explicit ResourceTreeFilterProxyModel(QObject *parent = nullptr);
	
protected:
	bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
};

#endif // RESOURCETREEFILTERPROXYMODEL_HPP
