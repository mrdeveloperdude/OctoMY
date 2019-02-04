#include "StreamCodecWidget.hpp"
#include "ui_StreamCodecWidget.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QImage>
#include <QPixmap>


StreamCodecWidget::StreamCodecWidget(QWidget *parent) :
	QWidget(parent),
	ui(OC_NEW Ui::StreamCodecWidget)
{
	OC_METHODGATE();
	ui->setupUi(this);
}

StreamCodecWidget::~StreamCodecWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}


void StreamCodecWidget::setImage(const QImage &im, quint8 num)
{
	OC_METHODGATE();
	setPixmap(QPixmap ::fromImage(im), num);
}

void StreamCodecWidget::setPixmap(const QPixmap &px, quint8 num)
{
	OC_METHODGATE();
	switch(num) {
	case(0):
		ui->labelPic1->setPixmap(px);
		break;
	case(1):
		ui->labelPic2->setPixmap(px);
		break;
	}
}

void StreamCodecWidget::on_pushButtonTrigger_clicked()
{
	OC_METHODGATE();
	emit trigger();
}
