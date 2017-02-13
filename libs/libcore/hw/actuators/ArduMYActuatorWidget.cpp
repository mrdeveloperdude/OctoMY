#include "ArduMYActuatorWidget.hpp"
#include "ui_ActuatorWidget.h"

#include "../libutil/utility/Utility.hpp"

#include "../arduino/ArduMYActuator.hpp"


#include <QDebug>
#include <QInputDialog>
#include <QRegularExpression>


quint32 ArduMYActuatorWidget::s_sid=0;

static QString ACTUATOR_UNNAMED="Unnamed";

ArduMYActuatorWidget::ArduMYActuatorWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::ActuatorWidget)
	, mID(s_sid++)
	, mLowTrim(INITIAL_SERVO_TRIM)
	, mHighTrim(INITIAL_SERVO_TRIM)
	, mTabIndex(0)
	, mActuator(nullptr)

{
	ui->setupUi(this);
	mTabIndex=(ui->tabWidget->count()-4);
	if(!connect(ui->numberEntryServoPosition,SIGNAL(valueChanged(int)),this,SLOT(onServoMoved()))) {
		qWarning()<<"ERROR: Could not connect";
	}

	if(!connect(ui->checkBoxEnabled,SIGNAL(clicked(bool)), this,SLOT(onServoKilled()))) {
		qWarning()<<"ERROR: Could not connect";
	}

	reset();

	// Remove the last 4 tabs

	for(quint32 i=0; i<4; ++i) {
		ui->tabWidget->removeTab(mTabIndex);
	}

	updateTabsVisibility();
	utility::pack(ui->tabWidget);
}

ArduMYActuatorWidget::~ArduMYActuatorWidget()
{
	delete ui;
}

void ArduMYActuatorWidget::reset()
{
	ui->tabWidget->setVisible(false);
	ui->spinBoxHighTrim->setValue(INITIAL_SERVO_TRIM);
	ui->spinBoxLowTrim->setValue(INITIAL_SERVO_TRIM);
	ui->checkBoxContinuous->setChecked(false);
	ui->checkBoxEnabled->setChecked(false);
	ui->checkBoxEnd->setChecked(false);
	ui->checkBoxLimitSwitchStart->setChecked(false);
	ui->checkBoxLinear->setChecked(false);
	ui->checkBoxPositionFeedback->setChecked(false);
	ui->checkBoxRelativeEncoder->setChecked(false);
	ui->checkBoxTachometer->setChecked(false);
	ui->checkBoxTachometer_2->setChecked(false);
	ui->checkBox->setChecked(false);
	ui->numberEntryServoPosition->setValue(50);
	ui->comboBoxActuatorRepresentation->setCurrentIndex(0);
	ui->comboBoxActuatorType->setCurrentIndex(0);
	ui->comboBoxLimitSwitchEndPin->setCurrentIndex(0);
	ui->comboBoxLimitSwitchStartPin->setCurrentIndex(0);
	ui->comboBoxPositionFeedbackPin->setCurrentIndex(0);
	ui->comboBoxRCInterfacePin->setCurrentIndex(0);
	ui->comboBoxRCInterfacePin_2->setCurrentIndex(0);
	ui->comboBoxRCInterfacePin_3->setCurrentIndex(0);
	ui->comboBoxRCInterfacePin_4->setCurrentIndex(0);
	ui->comboBoxRCInterfacePin_5->setCurrentIndex(0);
	ui->comboBoxRCInterfacePin_6->setCurrentIndex(0);
	ui->comboBoxRCInterfacePin_7->setCurrentIndex(0);
	ui->comboBoxRCInterfacePin_8->setCurrentIndex(0);
	ui->comboBoxRelativeEncoderPinA->setCurrentIndex(0);
	ui->comboBoxRelativeEncoderPinB->setCurrentIndex(0);
	ui->comboBoxTachometerPin->setCurrentIndex(0);
	ui->pushButtonName->setText(ACTUATOR_UNNAMED);
	ui->tabWidget->setCurrentIndex(0);
}


void ArduMYActuatorWidget::configure(ArduMYActuator *actuator)
{
	mActuator=actuator;
	if(mActuator) {
		ui->numberEntryServoPosition->setValue(mActuator->valueFloat()*(ui->numberEntryServoPosition->maximum()-ui->numberEntryServoPosition->minimum())+ui->numberEntryServoPosition->minimum());
		quint32 typeIndex=0;
		switch(mActuator->config.type) {
		case(TYPE_COUNT):
		default:
		case(RC_SERVO): {
			typeIndex=0;
		}
		break;
		case(DC_MOTOR): {
			typeIndex=1;
		}
		break;

		case(STEP_MOTOR): {
			typeIndex=2;
		}
		break;
		case(RELAY): {
			typeIndex=3;
		}
		break;
		}
		ui->comboBoxActuatorType->setCurrentIndex(typeIndex);
		updateTabsVisibility();
		reconfigureTrim();
		ui->labelID->setText(QString("%1").arg(mID, 2, 10, QChar('0')));
		QString name=QString::fromLatin1((char *)mActuator->config.nickName);
		if(name.isEmpty()) {
			name=ACTUATOR_UNNAMED;
		}
		ui->pushButtonName->setText(name);
	} else {
		reset();
	}
}

void ArduMYActuatorWidget::reconfigureTrim()
{

}

void ArduMYActuatorWidget::updateTabsVisibility()
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


void ArduMYActuatorWidget::disableServo()
{
	ui->checkBoxEnabled->setChecked(false);
}


void ArduMYActuatorWidget::on_pushButtonCenter_clicked()
{
	ui->numberEntryServoPosition->setValue((ui->numberEntryServoPosition->minimum()+ui->numberEntryServoPosition->maximum())/2);
}


void ArduMYActuatorWidget::onServoMoved()
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

void ArduMYActuatorWidget::onServoKilled()
{
	if(!ui->checkBoxEnabled->isChecked()) {
		emit servoKilled(mID);
	}
}


void ArduMYActuatorWidget::on_pushButtonConfigure_toggled(bool checked)
{
	ui->tabWidget->setVisible(checked);
	if(checked) {
		ui->tabWidget->setCurrentIndex(0);
		utility::pack(ui->tabWidget);
	}
}

void ArduMYActuatorWidget::on_spinBoxLowTrim_valueChanged(int val)
{
	mLowTrim=val;
	reconfigureTrim();
	onServoMoved();
}


void ArduMYActuatorWidget::on_spinBoxHighTrim_valueChanged(int val)
{
	mHighTrim=val;
	reconfigureTrim();
	onServoMoved();
}

void ArduMYActuatorWidget::on_tabWidget_currentChanged(int index)
{
	utility::pack(ui->tabWidget);
}

void ArduMYActuatorWidget::on_comboBoxActuatorType_currentIndexChanged(int index)
{
	updateTabsVisibility();
}

void ArduMYActuatorWidget::on_pushButtonResetGearRatio_clicked()
{
	ui->spinBoxGearRatioDenominator->setValue(1);
	ui->spinBoxGearRatioNominator->setValue(1);
}

void ArduMYActuatorWidget::on_pushButtonName_clicked()
{
	bool ok=false;
	QString name = QInputDialog::getText(this, tr("QInputDialog::getText()"), tr("User name:"), QLineEdit::Normal, ui->pushButtonName->text(), &ok);
	if (ok && !name.isEmpty()) {
		name=name.replace(QRegularExpression("[^a-zA-Z0-9_]*"),"").trimmed();
		//(Qt::ImhDigitsOnly|Qt::ImhUppercaseOnly|Qt::ImhLowercaseOnly)
		auto sz=sizeof(ArduMYActuatorConfig::nickName);
		name=name.left(sz-1);
		if(nullptr!=mActuator) {
			for(size_t i=0; i<sz; ++i) {
				mActuator->config.nickName[i]=name.at(i).toLatin1();
			}
			mActuator->config.nickName[sz]='\0';
		}
		if(name.isEmpty()) {
			name=ACTUATOR_UNNAMED;
		}
		ui->pushButtonName->setText(name);
	}
}
