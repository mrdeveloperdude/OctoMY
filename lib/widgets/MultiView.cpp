#include "MultiView.hpp"
#include "ui_MultiView.h"

MultiView::MultiView(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::MultiView)
{
	ui->setupUi(this);
}

MultiView::~MultiView()
{
	delete ui;
}
