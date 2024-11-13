#ifndef PAIRINGLISTWIDGET_HPP
#define PAIRINGLISTWIDGET_HPP

#include "uptime/ConfigureHelper.hpp"
#include "uptime/SharedPointerWrapper.hpp"
#include <QWidget>


class Node;
class PairingListModel;
class PairingEditButtonDelegate;

namespace Ui
{
class PairingListWidget;
}

class PairingListWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::PairingListWidget *ui;
	QSharedPointer<Node> mNode;
	PairingListModel *mList;
	PairingEditButtonDelegate *mDelegate;
	ConfigureHelper mConfigureHelper;

public:
	explicit PairingListWidget(QWidget *parent = nullptr);
	virtual ~PairingListWidget();

public:
	void configure(QSharedPointer<Node> node);

signals:
	void startEdit(const QString id);

};

#endif
// PAIRINGLISTWIDGET_HPP
