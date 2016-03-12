#include "NumberEntry.hpp"
#include "ui_NumberEntry.h"
#include "basic/Standard.hpp"
#include "basic/Settings.hpp"

NumberEntry::NumberEntry(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::NumberEntry)
{
	WWMETHODGATE();
	ui->setupUi(this);
	ui->spinBox->setFocusPolicy(Qt::StrongFocus);
	ui->horizontalSlider->setFocusPolicy(Qt::StrongFocus);
	if(!connect(ui->horizontalSlider,SIGNAL(valueChanged(int)),ui->spinBox,SLOT(setValue(int)))){
		qWarning()<<"ERROR: Could not connect";
	}
	if(!connect(ui->spinBox,SIGNAL(valueChanged(int)),ui->horizontalSlider,SLOT(setValue(int)))){
		qWarning()<<"ERROR: Could not connect";
	}
	if(!connect(ui->horizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(setValueInternal(int)))){
		qWarning()<<"ERROR: Could not connect";
	}
	if(!connect(ui->horizontalSlider,SIGNAL(valueChanged(int)),this,SIGNAL(valueChanged(int)))){
		qWarning()<<"ERROR: Could not connect";
	}
}


void NumberEntry::configure(int min, int max, int step, int val, QString suf, QString tip, QString key){
	WWMETHODGATE();
	qDebug()<<"CONFIG: MIN="<<min<<", MAX="<<max<<", STEP="<<step<<", VAL="<<val<<", SUFFIX="<<suf<< ", TIP"<<tip<< ", KEY"<<key;
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

void NumberEntry::setValue(const int val){
	WWMETHODGATE();
	ui->horizontalSlider->setValue(val);
}



void NumberEntry::setValueInternal(const int val){
	WWMETHODGATE();
	if(""!=k){
		Settings::getInstance().setCustomSettingLong(k,val);
	}
}


int NumberEntry::value(){
	WWMETHODGATE();
	return ui->horizontalSlider->value();
}


int NumberEntry::minimum(){
	WWMETHODGATE();
	return ui->horizontalSlider->minimum();
}


int NumberEntry::maximum(){
	WWMETHODGATE();
	return ui->horizontalSlider->maximum();
}

NumberEntry::~NumberEntry(){
	WWMETHODGATE();
	delete ui;
}

void NumberEntry::reloadData(){
	WWMETHODGATE();
	int val=ui->horizontalSlider->value();
	if(""!=k){
		val=Settings::getInstance().getCustomSettingLong(k,val);
	}
	ui->horizontalSlider->setValue(val);
}
