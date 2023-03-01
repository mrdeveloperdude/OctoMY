#include "LightTestWidget.hpp"
#include "ui_LightTestWidget.h"

#include "components/LightWidget.hpp"

#include <QColorDialog>
#include <QDebug>


LightTestWidget::LightTestWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::LightTestWidget)
	, mColor(Qt::green)
{
	ui->setupUi(this);

	const int min=15;
	const int inc=10;
	const int num=10;
	for(int x=0; x<num; ++x) {
		for(int y=0; y<num; ++y) {
			addLight(x,y,min+x*inc,min+y*inc);
		}
	}
	mTimer.setTimerType(Qt::PreciseTimer);
	mTimer.setInterval(400);
	connect(&mTimer, &QTimer::timeout, this, [=]() {
		for(LightWidget *light: mLights) {
			light->toggleLight();
		}
	});
}


void LightTestWidget::addLight(int x, int y,int w, int h)
{
	qDebug()<<"Adding light: "<<x<<","<<y<<" "<<w<<"x"<<h;
	LightWidget *light = new LightWidget(ui->widgetLights);
	QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
	sizePolicy1.setHorizontalStretch(0);
	sizePolicy1.setVerticalStretch(0);
	sizePolicy1.setHeightForWidth(light->sizePolicy().hasHeightForWidth());
	light->setSizePolicy(sizePolicy1);
	light->setMinimumSize(QSize(w, h));
	ui->gridLayout->addWidget(light, x, y);
	ui->widgetLights->setLayout(ui->gridLayout);
	mLights << light;
}


LightTestWidget::~LightTestWidget()
{
	delete ui;
}


void LightTestWidget::on_pushButtonSelectColor_clicked()
{
	mColor=QColorDialog::getColor(mColor);
	for(LightWidget *light: mLights) {
		light->setLightColor(mColor);
	}
}


void LightTestWidget::on_pushButtonLightOn_clicked()
{
	for(LightWidget *light: mLights) {
		light->setLightOn(true);
	}
}


void LightTestWidget::on_pushButtonLightOff_clicked()
{
	for(LightWidget *light: mLights) {
		light->setLightOn(false);
	}
}


void LightTestWidget::on_pushButtonToggleLight_clicked()
{
	for(LightWidget *light: mLights) {
		light->toggleLight();
	}
}


void LightTestWidget::on_pushButtonBlink_toggled(bool checked)
{
	checked?mTimer.start():mTimer.stop();
}
