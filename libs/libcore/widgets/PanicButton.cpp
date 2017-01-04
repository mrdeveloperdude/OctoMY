#include "PanicButton.hpp"
#include "ui_PanicButton.h"

#include "../libutil/utility/Standard.hpp"

PanicButton::PanicButton(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PanicButton)
{
	ui->setupUi(this);

	//Make panic button RED
	QPalette p=ui->pushButtonPanic->palette();
	p.setColor(QPalette::Button,"#5b0504");
	ui->pushButtonPanic->setPalette(p);

	// Forward all the button signals

	if(!connect(ui->pushButtonPanic, &QAbstractButton::pressed, this, &PanicButton::pressed, OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not forward pressed signal for panic button";
	}

	if(!connect(ui->pushButtonPanic, &QAbstractButton::released, this, &PanicButton::released, OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not forward released signal for panic button";
	}

	if(!connect(ui->pushButtonPanic, &QAbstractButton::clicked, this, &PanicButton::clicked, OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not forward click signal for panic button";
	}

	if(!connect(ui->pushButtonPanic, &QAbstractButton::toggled, this, &PanicButton::toggled, OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not forward toggled signal for panic button";
	}

}

PanicButton::~PanicButton()
{
	delete ui;
}



void PanicButton::setPanic(bool checked)
{
	ui->pushButtonPanic->setChecked(checked);
}


bool PanicButton::panic() const
{
	return ui->pushButtonPanic->isChecked();
}
