#ifndef NODENAVIGATION_HPP
#define NODENAVIGATION_HPP


#include "uptime/SharedPointerWrapper.hpp"


#include <QWidget>
#include <QPoint>

class Node;

namespace Ui {
class NodeNavigation;
}

class NodeNavigation : public QWidget
{
	Q_OBJECT
private:
	Ui::NodeNavigation *ui;
	
public:
	explicit NodeNavigation(QWidget *parent = nullptr);
	~NodeNavigation();

public:
	void setState(QString title, bool showBar=true, bool showBack=true, bool showMenu=true);
	QPoint menuPos();
	void configure(QSharedPointer<Node> node);

signals:
	void backClicked();
	void menuClicked();
};

#endif // NODENAVIGATION_HPP
