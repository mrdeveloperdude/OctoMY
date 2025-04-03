#ifndef MULTIVIEWFILTER_HPP
#define MULTIVIEWFILTER_HPP

#include <QSortFilterProxyModel>

class MultiViewFilter : public QSortFilterProxyModel
{
	Q_OBJECT
public:
	explicit MultiViewFilter(QObject *parent = nullptr);
	
protected:
	bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
};



#endif // MULTIVIEWFILTER_HPP
