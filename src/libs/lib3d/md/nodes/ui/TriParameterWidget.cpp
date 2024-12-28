#include "TriParameterWidget.hpp"
#include "ui_TriParameterWidget.h"

#include "md/nodes/Tri.hpp"
#include "md/nodes/Node.hpp"

TriParameterWidget::TriParameterWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::TriParameterWidget)
{
	ui->setupUi(this);
}


TriParameterWidget::~TriParameterWidget()
{
	delete ui;
}


void TriParameterWidget::setTri(QSharedPointer<Tri> tri){
	mTri = tri;
	setWindowTitle(QString("Tri <%1, %2, %3>").arg(mTri->node1()->name()).arg(mTri->node2()->name()).arg(mTri->node3()->name()));
}
