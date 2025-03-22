#include "UtilitiesWidget.hpp"
#include "ui_UtilitiesWidget.h"

#include "app/Settings.hpp"
#include "hub/Hub.hpp"
#include "log/LogDevice.hpp"

UtilitiesWidget::UtilitiesWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::UtilitiesWidget)
{
	ui->setupUi(this);
}

UtilitiesWidget::~UtilitiesWidget()
{
	delete ui;
}




QSharedPointer<Hub> UtilitiesWidget::hub(){
	return mHub;
}


void UtilitiesWidget::configure(QSharedPointer<Settings> _settings, QSharedPointer<Hub> hub){
	OC_METHODGATE();
	mHub = hub;
	auto settings = _settings.data();
	auto h = this->hub();
	auto cmd = h->context()->commandLine();
}


