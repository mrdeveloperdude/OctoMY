#include "PixViewer.hpp"
#include "ui_PixViewer.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QPixmap>
#include <QImage>

PixViewer::PixViewer(QWidget *parent, QString name) :
	QWidget(parent),
	ui(OC_NEW Ui::PixViewer)
{
	OC_METHODGATE();
	ui->setupUi(this);
	setWindowTitle(name);
}

PixViewer::~PixViewer()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}

void PixViewer::setImage(const QImage &im)
{
	OC_METHODGATE();
	setPixmap(QPixmap ::fromImage(im));
}

void PixViewer::setPixmap(const QPixmap &px)
{
	OC_METHODGATE();
	ui->label->setPixmap(px);
}


void PixViewer::on_pushButtonClose_clicked()
{
	OC_METHODGATE();
	close();
	emit closeClicked();
}
