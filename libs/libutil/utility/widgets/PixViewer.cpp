#include "PixViewer.hpp"
#include "ui_PixViewer.h"

#include <QPixmap>
#include <QImage>

PixViewer::PixViewer(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PixViewer)
{
	ui->setupUi(this);
}

PixViewer::~PixViewer()
{
	delete ui;
}

void PixViewer::setImage(const QImage &im)
{
	setPixmap(QPixmap ::fromImage(im));
}

void PixViewer::setPixmap(const QPixmap &px)
{
	ui->label->setPixmap(px);
}


void PixViewer::on_pushButtonClose_clicked()
{
	close();
}
