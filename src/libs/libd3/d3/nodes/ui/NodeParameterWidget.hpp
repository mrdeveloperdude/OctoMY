#ifndef NODEPARAMETERWIDGET_HPP
#define NODEPARAMETERWIDGET_HPP

#include <QWidget>
#include "uptime/SharedPointerWrapper.hpp"


class Node;
namespace Ui {
class NodeParameterWidget;
}

class NodeParameterWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::NodeParameterWidget *ui;
	QSharedPointer<Node> mNode;

public:
	explicit NodeParameterWidget(QWidget *parent = nullptr);
	~NodeParameterWidget();

public:
	void updateNode();
	void setNode(QSharedPointer<Node> node);
private slots:
	void anchoredChanged(bool anchored);
	void massChanged(double mass);
};

#endif // NODEPARAMETERWIDGET_HPP
