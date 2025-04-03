#ifndef INTERNALRESOURCEMANAGER_HPP
#define INTERNALRESOURCEMANAGER_HPP

#include <QWidget>


class ResourceTreeModel;
class ResourceTreeFilterProxyModel;

namespace Ui {
class InternalResourceManager;
}

class InternalResourceManager : public QWidget
{
	Q_OBJECT
private:
	Ui::InternalResourceManager *ui;
	ResourceTreeModel *mModel;
	ResourceTreeFilterProxyModel *mProxyModel;

public:
	explicit InternalResourceManager(QWidget *parent = nullptr);
	~InternalResourceManager();

public slots:
	void search(const QString &filter);

};

#endif // INTERNALRESOURCEMANAGER_HPP
