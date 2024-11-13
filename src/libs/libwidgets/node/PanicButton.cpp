#include "PanicButton.hpp"
#include "ui_PanicButton.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QDebug>

PanicButton::PanicButton(QWidget *parent)
	: QAbstractButton(parent)
	, ui(OC_NEW Ui::PanicButton)
{
	OC_METHODGATE();
	ui->setupUi(this);

	mTimer.setInterval(1000/5); //Multiple of 60Hz gives stutter free animation
	mTimer.setTimerType(Qt::PreciseTimer);
	connect(&mTimer, &QTimer::timeout, this, &PanicButton::timeout);

	//Make panic button RED
	QPalette p = ui->pushButtonPanic->palette();
	p.setColor(QPalette::Button, 0x5b0504);
	mPaletteNormal = p;
	ui->pushButtonPanic->setPalette(p);
	p.setColor(QPalette::ButtonText, 0xFF0000);
	mPaletteBlink = p;
}


PanicButton::~PanicButton()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}


bool PanicButton::panic() const
{
	OC_METHODGATE();
	return mPanic;
}


void PanicButton::setPanic(bool newPanic)
{
	OC_METHODGATE();
	const auto old = mPanic;
	mPanic = newPanic;
	qDebug() << "PANIC " << old << " --> " << mPanic;
	if(old != mPanic){
		ui->pushButtonPanic->setChecked(mPanic);
		mPanic ? mTimer.start():mTimer.stop();
		mFlipFlop = mPanic;
		timeout();
		emit panicChanged(mPanic);
	}
}


void PanicButton::timeout()
{
	OC_METHODGATE();
	ui->pushButtonPanic->setPalette(mFlipFlop ? mPaletteBlink : mPaletteNormal);
	mFlipFlop =! mFlipFlop;
}
