#include "PixViewer.hpp"
#include "ui_PixViewer.h"

#include "utility/Standard.hpp"

#include <QPixmap>
#include <QImage>

PixViewer::PixViewer(QWidget *parent, QString name) :
	QWidget(parent),
	ui(OC_NEW Ui::PixViewer)
{
	ui->setupUi(this);
	setWindowTitle(name);
}

PixViewer::~PixViewer()
{
	delete ui;
	ui=nullptr;
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
	emit closeClicked();
}
