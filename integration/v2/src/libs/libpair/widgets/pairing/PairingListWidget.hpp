#ifndef PAIRINGLISTWIDGET_HPP
#define PAIRINGLISTWIDGET_HPP

#include "uptime/ConfigureHelper.hpp"

#include <QWidget>
#include <QSharedPointer>

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

};

#endif
// PAIRINGLISTWIDGET_HPP
