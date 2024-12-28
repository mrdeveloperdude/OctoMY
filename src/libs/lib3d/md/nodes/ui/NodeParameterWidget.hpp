#ifndef NODEPARAMETERWIDGET_HPP
#define NODEPARAMETERWIDGET_HPP

#include <QWidget>
#include <QSharedPointer>


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
	void setNode(QSharedPointer<Node> node);
};

#endif // NODEPARAMETERWIDGET_HPP
