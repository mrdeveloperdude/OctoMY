#include "StreamCodecWidget.hpp"
#include "ui_StreamCodecWidget.h"

#include <QImage>
#include <QPixmap>


StreamCodecWidget::StreamCodecWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::StreamCodecWidget)
{
	ui->setupUi(this);
}

StreamCodecWidget::~StreamCodecWidget()
{
	delete ui;
	ui=nullptr;
}


void StreamCodecWidget::setImage(const QImage &im, quint8 num)
{
	setPixmap(QPixmap ::fromImage(im), num);
}

void StreamCodecWidget::setPixmap(const QPixmap &px, quint8 num)
{
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
	emit trigger();
}
