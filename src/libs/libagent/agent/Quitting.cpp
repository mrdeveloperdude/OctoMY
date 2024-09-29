#include "Quitting.hpp"
#include "ui_Quitting.h"

Quitting::Quitting(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::Quitting)
{
	ui->setupUi(this);
}


Quitting::~Quitting()
{
	delete ui;
}


void Quitting::cancel(){
	emit done(false);
}


void Quitting::doQuit(){
	emit done(true);
}
