#include "PanicButton.hpp"
#include "ui_PanicButton.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "uptime/ConnectionType.hpp"

#include <QDebug>

PanicButton::PanicButton(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::PanicButton)
	, mFlipFlop(false)
{
	OC_METHODGATE();
	ui->setupUi(this);

	mTimer.setInterval(1000/5); //Multiple of 60Hz gives stutter free animation
	mTimer.setTimerType(Qt::PreciseTimer);
	connect(&mTimer,SIGNAL(timeout()),this,SLOT(onTimeout()));

	//Make panic button RED
	QPalette p=ui->pushButtonPanic->palette();
	p.setColor(QPalette::Button,"#5b0504");
	mPaletteNormal=p;
	ui->pushButtonPanic->setPalette(p);
	p.setColor(QPalette::ButtonText,"#FF0000");
	mPaletteBlink=p;

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

	if(!connect(ui->pushButtonPanic, &QAbstractButton::toggled, this, &PanicButton::onToggled, OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect toggled signal for panic button";
	}

}

PanicButton::~PanicButton()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}



void PanicButton::setPanic(bool checked)
{
	OC_METHODGATE();
	ui->pushButtonPanic->setChecked(checked);
	mFlipFlop=checked;
	onTimeout();
	checked?mTimer.start():mTimer.stop();
}


bool PanicButton::panic() const
{
	OC_METHODGATE();
	return ui->pushButtonPanic->isChecked();
}

void PanicButton::onToggled(bool checked)
{
	OC_METHODGATE();
	setPanic(checked);
}

void PanicButton::onTimeout()
{
	OC_METHODGATE();
	ui->pushButtonPanic->setPalette(mFlipFlop?mPaletteBlink:mPaletteNormal);
	mFlipFlop=!mFlipFlop;
}
