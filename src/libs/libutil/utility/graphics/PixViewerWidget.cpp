#include "PixViewerWidget.hpp"
#include "ui_PixViewerWidget.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QPixmap>
#include <QImage>

PixViewerWidget::PixViewerWidget(QWidget *parent, QString name) :
	QWidget(parent),
	ui(OC_NEW Ui::PixViewerWidget)
{
	OC_METHODGATE();
	ui->setupUi(this);
	setWindowTitle(name);
}

PixViewerWidget::~PixViewerWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}

void PixViewerWidget::setImage(const QImage &im)
{
	OC_METHODGATE();
	setPixmap(QPixmap ::fromImage(im));
}

void PixViewerWidget::setPixmap(const QPixmap &px)
{
	OC_METHODGATE();
	ui->label->setPixmap(px);
}


void PixViewerWidget::on_pushButtonClose_clicked()
{
	OC_METHODGATE();
	close();
	emit closeClicked();
}
