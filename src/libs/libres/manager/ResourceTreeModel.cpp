#include "ResourceTreeModel.hpp"
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QIcon>
#include <QPixmap>
#include <QMimeDatabase>
#include <QMimeType>

// Internal helper class representing a single tree node.
class ResourceTreeItem
{
public:
	explicit ResourceTreeItem(const QString &name, const QString &fullPath, bool isDir, ResourceTreeItem *parent = nullptr)
		: m_name(name)
		, m_fullPath(fullPath)
		, m_isDir(isDir)
		, m_parent(parent)
		, m_fileSize(0)
	{
		// For directories, set a generic mime type.
		if (m_isDir) {
			m_mimeType = QStringLiteral("Directory");
			m_fileSize = 0;
		} else {
			QFileInfo info(fullPath);
			m_fileSize = info.size();
			QMimeDatabase db;
			m_mimeType = db.mimeTypeForFile(info).name();
		}
	}
	
	~ResourceTreeItem() { qDeleteAll(m_children); }
	
	void appendChild(ResourceTreeItem *child) { m_children.append(child); }
	ResourceTreeItem *child(int row) const { return m_children.value(row); }
	int childCount() const { return m_children.count(); }
	int row() const { return m_parent ? m_parent->m_children.indexOf(const_cast<ResourceTreeItem*>(this)) : 0; }
	ResourceTreeItem *parent() { return m_parent; }
	QString name() const { return m_name; }
	QString fullPath() const { return m_fullPath; }
	bool isDir() const { return m_isDir; }
	QString mimeType() const { return m_mimeType; }
	qint64 fileSize() const { return m_fileSize; }
	
public:
	QVector<ResourceTreeItem*> m_children;
	
private:
	QString m_name;
	QString m_fullPath;
	bool m_isDir;
	ResourceTreeItem *m_parent;
	QString m_mimeType;
	qint64 m_fileSize;
};

static QString formatFileSize(qint64 size)
{
	if (size < 1024)
		return QString::number(size) + " B";
	else if (size < 1024 * 1024)
		return QString::number(size / 1024.0, 'f', 2) + " KB";
	else if (size < 1024LL * 1024LL * 1024LL)
		return QString::number(size / (1024.0 * 1024.0), 'f', 2) + " MB";
	else
		return QString::number(size / (1024.0 * 1024.0 * 1024.0), 'f', 2) + " GB";
}

ResourceTreeModel::ResourceTreeModel(QObject *parent)
	: QAbstractItemModel(parent)
{
	// The root represents the entire resource system (":/")
	m_rootItem = new ResourceTreeItem(":/", ":/", true);
	reload();
}

ResourceTreeModel::~ResourceTreeModel()
{
	delete m_rootItem;
}

QModelIndex ResourceTreeModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();
	
	ResourceTreeItem *parentItem;
	if (!parent.isValid())
		parentItem = m_rootItem;
	else
		parentItem = static_cast<ResourceTreeItem*>(parent.internalPointer());
	
	ResourceTreeItem *childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	return QModelIndex();
}

QModelIndex ResourceTreeModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();
	
	ResourceTreeItem *childItem = static_cast<ResourceTreeItem*>(index.internalPointer());
	ResourceTreeItem *parentItem = childItem->parent();
	
	if (parentItem == m_rootItem || !parentItem)
		return QModelIndex();
	
	return createIndex(parentItem->row(), 0, parentItem);
}

int ResourceTreeModel::rowCount(const QModelIndex &parent) const
{
	ResourceTreeItem *parentItem;
	if (parent.column() > 0)
		return 0;
	if (!parent.isValid())
		parentItem = m_rootItem;
	else
		parentItem = static_cast<ResourceTreeItem*>(parent.internalPointer());
	
	return parentItem->childCount();
}

int ResourceTreeModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	// We have three columns: Name, Type, and Size.
	return 3;
}

QVariant ResourceTreeModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();
	
	ResourceTreeItem *item = static_cast<ResourceTreeItem*>(index.internalPointer());
	
	if (role == Qt::DisplayRole)
	{
		switch (index.column()) {
		case 0:
			return item->name();
		case 1:
			return item->mimeType();
		case 2:
			// For directories, leave the size column empty.
			return item->isDir() ? QVariant() : formatFileSize(item->fileSize());
		}
	}
	else if (role == Qt::DecorationRole && index.column() == 0)
	{
		// Provide an icon for the item.
		if (item->isDir()) {
			return QIcon::fromTheme("folder");
		} else {
			// If the file is an image, return a thumbnail.
			if (item->mimeType().startsWith("image/")) {
				QPixmap pix(item->fullPath());
				if (!pix.isNull())
					return pix.scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation);
			}
			// Otherwise, return a generic file icon.
			return QIcon::fromTheme("text-x-generic");
		}
	}
	
	return QVariant();
}

Qt::ItemFlags ResourceTreeModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant ResourceTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		switch (section) {
		case 0:
			return tr("Name");
		case 1:
			return tr("Type");
		case 2:
			return tr("Size");
		}
	}
	return QAbstractItemModel::headerData(section, orientation, role);
}

void ResourceTreeModel::reload()
{
	beginResetModel();
	delete m_rootItem;
	m_rootItem = new ResourceTreeItem(":/", ":/", true);
	setupModelData(":/", m_rootItem);
	endResetModel();
}

void ResourceTreeModel::setupModelData(const QString &path, ResourceTreeItem *parentItem)
{
	QDir dir(path);
	// List all entries (files and directories) excluding "." and ".."
	QFileInfoList infos = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries, QDir::Name);
	for (const QFileInfo &info : infos)
	{
		QString fileName = info.fileName();
		// Normalize the full resource path
		QString fullPath = QDir::cleanPath(info.absoluteFilePath());
		bool isDir = info.isDir();
		ResourceTreeItem *item = new ResourceTreeItem(fileName, fullPath, isDir, parentItem);
		parentItem->appendChild(item);
		if (isDir)
		{
			setupModelData(fullPath, item);
		}
	}
}
