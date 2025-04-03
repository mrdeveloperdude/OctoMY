#ifndef RESOURCETREEMODEL_HPP
#define RESOURCETREEMODEL_HPP

#include <QAbstractItemModel>
#include <QString>
#include <QVector>
#include <QVariant>

class ResourceTreeItem;

class ResourceTreeModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	explicit ResourceTreeModel(QObject *parent = nullptr);
	~ResourceTreeModel();
	
	// QAbstractItemModel interface:
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	
	// Re-scan the resource system (":/") and rebuild the model tree.
	void reload();
	
private:
	ResourceTreeItem *m_rootItem;
	void setupModelData(const QString &path, ResourceTreeItem *parentItem);
};

#endif // RESOURCETREEMODEL_HPP
