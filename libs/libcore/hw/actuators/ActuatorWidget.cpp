#include "ActuatorWidget.hpp"
#include "ui_ActuatorWidget.h"

#include "../libutil/utility/Utility.hpp"


#include <QDebug>


quint32 ActuatorWidget::s_sid=0;

ActuatorWidget::ActuatorWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::ActuatorWidget)
	, mID(s_sid++)
	, mLowTrim(INITIAL_SERVO_TRIM)
	, mHighTrim(INITIAL_SERVO_TRIM)
	, settings(nullptr)
	, mTabIndex(0)

{
	ui->setupUi(this);
	mTabIndex=(ui->tabWidget->count()-4);
	if(!connect(ui->numberEntryServoPosition,SIGNAL(valueChanged(int)),this,SLOT(onServoMoved()))) {
		qWarning()<<"ERROR: Could not connect";
	}

	if(!connect(ui->checkBoxEnabled,SIGNAL(clicked(bool)), this,SLOT(onServoKilled()))) {
		qWarning()<<"ERROR: Could not connect";
	}
	configure(nullptr, mID,mLowTrim,mHighTrim);
	ui->spinBoxHighTrim->setValue(mLowTrim);
	ui->spinBoxLowTrim->setValue(mHighTrim);
	ui->stackedWidget->setCurrentWidget(ui->pageNonContinuous);

	// Remove the last 4 tabs

	for(quint32 i=0; i<4; ++i) {
		ui->tabWidget->removeTab(mTabIndex);
	}

	updateTabsVisibility();
	utility::pack(ui->tabWidget);
	utility::pack(ui->stackedWidget);
}

ActuatorWidget::~ActuatorWidget()
{
	delete ui;
}


void ActuatorWidget::configure(Settings *s, quint32 id, int low_trim, int high_trim)
{
	settings=s;
	mLowTrim=low_trim;
	mHighTrim=high_trim;
	mID=id;
	ui->numberEntryServoPosition->setValue(1500);
	reconfigureTrim();
	ui->labelID->setText(QString("%1").arg(mID, 2, 10, QChar('0')));
}

void ActuatorWidget::reconfigureTrim()
{
	ui->numberEntryServoPosition->configure(settings, 500-mLowTrim,2500+mHighTrim,50,ui->numberEntryServoPosition->value()," µs","Servo position","servo_position");
}

void ActuatorWidget::updateTabsVisibility()
{
	setUpdatesEnabled(false);
	// Remove last active "optional tab"
	const quint32 current=ui->tabWidget->currentIndex();
	ui->tabWidget->removeTab(mTabIndex);
	switch(ui->comboBoxActuatorType->currentIndex()) {
	case(0): {
		//QIcon icon; icon.addFile(QStringLiteral(":/icons/rc_servo.svg"), QSize(), QIcon::Normal, QIcon::Off);ui->tabWidget->addTab(ui->tabRCServo, icon, "RC Servo");
		ui->tabWidget->addTab(ui->tabRCServo, "RC Servo");
	}
	break;
	case(1): {
		//QIcon icon;		icon.addFile(QStringLiteral(":/icons/dc_motor.svg"), QSize(), QIcon::Normal, QIcon::Off);ui->tabWidget->addTab(ui->tabDCMotor, icon, "DC Motor");
		ui->tabWidget->addTab(ui->tabDCMotor,  "DC Motor");
	}
	break;
	case(2): {
		//QIcon icon;	icon.addFile(QStringLiteral(":/icons/stepper_motor.svg"), QSize(), QIcon::Normal, QIcon::Off); ui->tabWidget->addTab(ui->tabStepMotor, icon, "Step Motor");
		ui->tabWidget->addTab(ui->tabStepMotor, "Step Motor");
	}
	break;
	case(3): {
		//QIcon icon;		icon.addFile(QStringLiteral(":/icons/relay.svg"), QSize(), QIcon::Normal, QIcon::Off); ui->tabWidget->addTab(ui->tabRelaySwitch, icon, "Relay");
		ui->tabWidget->addTab(ui->tabRelaySwitch, "Relay");
	}
	break;
	}
	ui->tabWidget->setCurrentIndex(current);
	setUpdatesEnabled(true);
	utility::pack(ui->tabWidget);
}


void ActuatorWidget::disableServo()
{
	ui->checkBoxEnabled->setChecked(false);
}


void ActuatorWidget::on_pushButtonCenter_clicked()
{
	ui->numberEntryServoPosition->setValue((ui->numberEntryServoPosition->minimum()+ui->numberEntryServoPosition->maximum())/2);
}


void ActuatorWidget::onServoMoved()
{
	qreal val=ui->numberEntryServoPosition->value();
	const qreal min=ui->numberEntryServoPosition->minimum();
	const qreal max=ui->numberEntryServoPosition->maximum();
	val-=min;
	val/=(max-min);
	ui->checkBoxEnabled->setChecked(true);
	emit servoMoved(mID, val);
	//pos[0]=(val*2.0)-1.0;
	//hexy->move(pos,0b00000000000000000000000000000001);
}

void ActuatorWidget::onServoKilled()
{
	if(!ui->checkBoxEnabled->isChecked()) {
		emit servoKilled(mID);
	}
}


void ActuatorWidget::on_pushButtonConfigure_toggled(bool checked)
{
	if(checked) {
		ui->tabWidget->setCurrentIndex(0);
		utility::pack(ui->tabWidget);
	}
	ui->stackedWidget->setCurrentWidget(checked?ui->pageConfigure:ui->pageNonContinuous);
	utility::pack(ui->stackedWidget);
}

void ActuatorWidget::on_spinBoxLowTrim_valueChanged(int val)
{
	mLowTrim=val;
	reconfigureTrim();
	onServoMoved();
}


void ActuatorWidget::on_spinBoxHighTrim_valueChanged(int val)
{
	mHighTrim=val;
	reconfigureTrim();
	onServoMoved();
}

void ActuatorWidget::on_tabWidget_currentChanged(int index)
{
	utility::pack(ui->tabWidget);
}

void ActuatorWidget::on_comboBoxActuatorType_currentIndexChanged(int index)
{
	updateTabsVisibility();
}

void ActuatorWidget::on_pushButtonResetGearRatio_clicked()
{
	ui->spinBoxGearRatioDenominator->setValue(1);
	ui->spinBoxGearRatioNominator->setValue(1);
}
