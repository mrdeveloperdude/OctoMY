#include "NodeParameterWidget.hpp"
#include "ui_NodeParameterWidget.h"

#include "d3/nodes/Node.hpp"

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

void NodeParameterWidget::updateNode(){
	ui->labelName->setText(mNode->name());
	ui->labelID->setText(mNode->id());
	ui->doubleSpinBoxMass->setValue(mNode->mass());
	ui->checkBoxAnchored->setChecked(mNode->isAnchored());
}


void NodeParameterWidget::setNode(QSharedPointer<Node> node){
	mNode = node;
	setWindowTitle(QString("Node %1").arg(mNode->name()));
	updateNode();
}


void NodeParameterWidget::anchoredChanged(bool anchored){
	mNode->setAnchored(anchored);
}

void NodeParameterWidget::massChanged(double mass){
	mNode->setMass(mass);
}
