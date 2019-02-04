#include "NumberEntry.hpp"
#include "ui_NumberEntry.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include "app/Settings.hpp"

#include <QDebug>

NumberEntry::NumberEntry(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::NumberEntry)
	, settings(nullptr)
{
	OC_METHODGATE();
	ui->setupUi(this);
	ui->spinBox->setFocusPolicy(Qt::StrongFocus);
	ui->horizontalSlider->setFocusPolicy(Qt::StrongFocus);
	if(!connect(ui->horizontalSlider,SIGNAL(valueChanged(int)),ui->spinBox,SLOT(setValue(int)))) {
		qWarning()<<"ERROR: Could not connect";
	}
	if(!connect(ui->spinBox,SIGNAL(valueChanged(int)),ui->horizontalSlider,SLOT(setValue(int)))) {
		qWarning()<<"ERROR: Could not connect";
	}
	if(!connect(ui->horizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(setValueInternal(int)))) {
		qWarning()<<"ERROR: Could not connect";
	}
	if(!connect(ui->horizontalSlider,SIGNAL(valueChanged(int)),this,SIGNAL(valueChanged(int)))) {
		qWarning()<<"ERROR: Could not connect";
	}
}


void NumberEntry::configure(Settings *s, int min, int max, int step, int val, QString suf, QString tip, QString key)
{
	OC_METHODGATE();
	settings=s;
	//qDebug()<<"CONFIG: MIN="<<min<<", MAX="<<max<<", STEP="<<step<<", VAL="<<val<<", SUFFIX="<<suf<< ", TIP"<<tip<< ", KEY"<<key;
	ui->spinBox->setRange(min,max);
	ui->spinBox->setSuffix(suf);
	ui->spinBox->setSingleStep(1);
	ui->spinBox->setToolTip(tip);
	ui->horizontalSlider->setRange(min,max);
	ui->horizontalSlider->setPageStep(step);
	ui->horizontalSlider->setToolTip(tip);
	ui->horizontalSlider->setTickInterval(step);
	ui->spinBox->setValue(val+1);
	ui->horizontalSlider->setValue(val+1);
	k=key.trimmed();
	setValue(val);
	reloadData();
}

void NumberEntry::setValue(const int val)
{
	OC_METHODGATE();
	ui->horizontalSlider->setValue(val);
}



void NumberEntry::setValueInternal(const int val)
{
	OC_METHODGATE();
	if(""!=k && nullptr!=settings) {
		settings->setCustomSettingLong(k,val);
	}
}


int NumberEntry::value()
{
	OC_METHODGATE();
	return ui->horizontalSlider->value();
}


int NumberEntry::minimum()
{
	OC_METHODGATE();
	return ui->horizontalSlider->minimum();
}


int NumberEntry::maximum()
{
	OC_METHODGATE();
	return ui->horizontalSlider->maximum();
}

NumberEntry::~NumberEntry()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}

void NumberEntry::reloadData()
{
	OC_METHODGATE();
	qint64 val=static_cast<qint64>(ui->horizontalSlider->value());
	if(""!=k && nullptr!=settings) {
		val=settings->getCustomSettingLong(k, val);
	}
	ui->horizontalSlider->setValue(static_cast<int>(val));
}
