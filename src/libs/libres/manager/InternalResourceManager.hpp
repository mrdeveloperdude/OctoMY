#ifndef INTERNALRESOURCEMANAGER_HPP
#define INTERNALRESOURCEMANAGER_HPP

#include <QWidget>

namespace Ui {
class InternalResourceManager;
}

class InternalResourceManager : public QWidget
{
	Q_OBJECT

public:
	explicit InternalResourceManager(QWidget *parent = nullptr);
	~InternalResourceManager();

private:
	Ui::InternalResourceManager *ui;
};

#endif // INTERNALRESOURCEMANAGER_HPP
