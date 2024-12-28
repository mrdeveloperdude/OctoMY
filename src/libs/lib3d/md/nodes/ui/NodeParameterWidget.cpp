#include "NodeParameterWidget.hpp"
#include "ui_NodeParameterWidget.h"

#include "md/nodes/Node.hpp"

NodeParameterWidget::NodeParameterWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::NodeParameterWidget)
{
	ui->setupUi(this);
}

NodeParameterWidget::~NodeParameterWidget()
{
	delete ui;
}

void NodeParameterWidget::setNode(QSharedPointer<Node> node){
	mNode = node;
	setWindowTitle(QString("Node %1").arg(mNode->name()));
}
