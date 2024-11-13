#include "ArduMYActuatorWidget.hpp"
#include "ui_ArduMYActuatorWidget.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "uptime/ConnectionType.hpp"

//#include "utility/string/String.hpp"
#include "utility/ui/Ui.hpp"

#include "ardumy_arduino/actuator/ArduMYActuator.hpp"
#include "ardumy/ArduMYTypeConversions.hpp"


#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QRegularExpression>


static QString ACTUATOR_UNNAMED="Unnamed";

ArduMYActuatorWidget::ArduMYActuatorWidget(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::ArduMYActuatorWidget)
	, mID(0)
	, mLowTrim(INITIAL_SERVO_TRIM)
	, mHighTrim(INITIAL_SERVO_TRIM)
	, mTabIndex(0)
	, mActuator(nullptr)

{
	OC_METHODGATE();
	ui->setupUi(this);
	mTabIndex=(ui->tabWidget->count()-4);
	if(!connect(ui->numberEntryActuatorPosition,SIGNAL(valueChanged(int)),this,SLOT(onActuatorMoved()), OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}

	if(!connect(ui->checkBoxEnabled,SIGNAL(clicked()), this,SLOT(onActuatorLimped()), OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}

	reset();

	// Remove the last 4 tabs

	for(quint32 i=0; i<4; ++i) {
		ui->tabWidget->removeTab(mTabIndex);
	}

	updateTabsVisibility();
	utility::ui::pack(ui->tabWidget);
}

ArduMYActuatorWidget::~ArduMYActuatorWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}

void ArduMYActuatorWidget::reset()
{
	OC_METHODGATE();
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
	ui->numberEntryActuatorPosition->setValue(50);
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


void ArduMYActuatorWidget::configure(ArduMYActuator *actuator, quint32 id)
{
	OC_METHODGATE();
	mActuator=actuator;
	mID=id;
	if(mActuator) {
		const int val=mActuator->valueFloat()*(ui->numberEntryActuatorPosition->maximum()-ui->numberEntryActuatorPosition->minimum())+ui->numberEntryActuatorPosition->minimum();
		ui->numberEntryActuatorPosition->setValue(val);
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
		quint32 representationIndex=0;
		switch(mActuator->config.representation) {
		case(VALREP_BIT): {
			representationIndex=0;
		}
		break;
		case(VALREP_BYTE): {
			representationIndex=1;
		}
		break;
		default:
		case(VALREP_REPRESENTATION_COUNT):
		case(VALREP_WORD): {
			representationIndex=2;
		}
		break;
		case(VALREP_DOUBLE_WORD): {
			representationIndex=3;
		}
		break;
		case(VALREP_QUAD_WORD): {
			representationIndex=4;
		}
		break;
		case(VALREP_SINGLE_FLOAT): {
			representationIndex=5;
		}
		break;
		case(VALREP_DOUBLE_FLOAT): {
			representationIndex=6;
		}
		break;
		}
		qDebug()<<"CONFIGURING WIDGET WITH INDEX: "<<id<<" WITH REP="<<representationIndex<<", TYPE="<<typeIndex<<" AND VALUE "<<val;
		ui->comboBoxActuatorRepresentation->setCurrentIndex(representationIndex);
		updateTabsVisibility();
		reconfigureTrim();
		ui->labelID->setText(QString("%1").arg(mID, 2, 10, QChar('0')));
		QString name=QString::fromLatin1((char *)mActuator->config.nickName);
		if(name.isEmpty()) {
			name=ACTUATOR_UNNAMED;
		}
		ui->pushButtonName->setText(name);
	} else {
		qDebug()<<"CONFIGURING WIDGET WITH INDEX: "<<id<<" RESET";
		reset();
	}
}

void ArduMYActuatorWidget::reconfigureTrim()
{
	OC_METHODGATE();

}

void ArduMYActuatorWidget::updateTabsVisibility()
{
	OC_METHODGATE();
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
	utility::ui::pack(ui->tabWidget);
}


void ArduMYActuatorWidget::disableServo()
{
	OC_METHODGATE();
	ui->checkBoxEnabled->setChecked(false);
}


void ArduMYActuatorWidget::on_pushButtonCenter_clicked()
{
	OC_METHODGATE();
	ui->numberEntryActuatorPosition->setValue((ui->numberEntryActuatorPosition->minimum()+ui->numberEntryActuatorPosition->maximum())/2);
}


void ArduMYActuatorWidget::onActuatorMoved()
{
	OC_METHODGATE();
	const qreal raw=ui->numberEntryActuatorPosition->value();
	qreal val=raw;
	const qreal min=ui->numberEntryActuatorPosition->minimum();
	const qreal max=ui->numberEntryActuatorPosition->maximum();
	const qreal mid=(max+min)/2.0f;
	val-=min;
	val/=(max-min);
	ui->checkBoxEnabled->setChecked(true);
	if(nullptr!=mActuator) {

		switch(mActuator->config.representation) {
		case(VALREP_BIT): {
			mActuator->state.value.bit=val>mid;
		}
		break;
		case(VALREP_BYTE): {
			mActuator->state.value.byte=0xFF*val;
		}
		break;
		case(VALREP_WORD): {
			mActuator->state.value.word=0xFFFF*val;
		}
		break;
		case(VALREP_DOUBLE_WORD): {
			mActuator->state.value.doubleWord=0xFFFFFFFF*val;
		}
		break;
		case(VALREP_QUAD_WORD): {
			mActuator->state.value.quadWord=0xFFFFFFFFFFFFFFFF*val;
		}
		break;
		default:// Fall back to single precision float for now
		case(VALREP_SINGLE_FLOAT): {
			mActuator->state.value.singlePrecision=val;
		}
		break;
		case(VALREP_DOUBLE_FLOAT): {
			mActuator->state.value.doublePrecision=val;
		}
		break;
		}
		//qDebug()<<"MOVED TO "<<val;
		mActuator->state.setDirty(true);
	}
	emit actuatorMoved(mID, val);
}

void ArduMYActuatorWidget::onActuatorLimped()
{
	OC_METHODGATE();
	emit actuatorLimped(mID, !ui->checkBoxEnabled->isChecked());
}


void ArduMYActuatorWidget::on_pushButtonConfigure_toggled(bool checked)
{
	OC_METHODGATE();
	ui->tabWidget->setVisible(checked);
	if(checked) {
		ui->tabWidget->setCurrentIndex(0);
		utility::ui::pack(ui->tabWidget);
	}
}

void ArduMYActuatorWidget::on_spinBoxLowTrim_valueChanged(int val)
{
	OC_METHODGATE();
	mLowTrim=val;
	reconfigureTrim();
	onActuatorMoved();
}


void ArduMYActuatorWidget::on_spinBoxHighTrim_valueChanged(int val)
{
	OC_METHODGATE();
	mHighTrim=val;
	reconfigureTrim();
	onActuatorMoved();
}

void ArduMYActuatorWidget::on_tabWidget_currentChanged(int index)
{
	OC_METHODGATE();
	utility::ui::pack(ui->tabWidget);
}

void ArduMYActuatorWidget::on_comboBoxActuatorType_currentIndexChanged(int index)
{
	OC_METHODGATE();
	updateTabsVisibility();
}

void ArduMYActuatorWidget::on_pushButtonResetGearRatio_clicked()
{
	OC_METHODGATE();
	ui->spinBoxGearRatioDenominator->setValue(1);
	ui->spinBoxGearRatioNominator->setValue(1);
}

void ArduMYActuatorWidget::on_pushButtonName_clicked()
{
	OC_METHODGATE();
	bool ok=false;
	QString name = QInputDialog::getText(this, tr("QInputDialog::getText()"), tr("User name:"), QLineEdit::Normal, ui->pushButtonName->text(), &ok);
	if (ok && !name.isEmpty()) {
		// Reduce any sized whitespace to single space
		name=name.replace(QRegularExpression("\\s+")," ");
		// Reduce all unsupported charecters to nothing
		name=name.replace(QRegularExpression("[^a-zA-Z0-9 ]*"),"").trimmed();
		//(Qt::ImhDigitsOnly|Qt::ImhUppercaseOnly|Qt::ImhLowercaseOnly)
		if(nullptr!=mActuator) {
			ardumyActuatorNameFromString(mActuator->config, name);
		}
		if(name.isEmpty()) {
			name=ACTUATOR_UNNAMED;
		}
		ui->pushButtonName->setText(name);
	}
}

void ArduMYActuatorWidget::on_comboBoxActuatorRepresentation_currentIndexChanged(int index)
{
	OC_METHODGATE();
	if(nullptr!=mActuator) {
		if(index<0) {
			index=(int)VALREP_REPRESENTATION_COUNT;
		} else if(index>(int)VALREP_REPRESENTATION_COUNT) {
			index=(int)VALREP_REPRESENTATION_COUNT;
		}
		mActuator->config.representation=(ArduMYActuatorValueRepresentation)index;
	}

}

void ArduMYActuatorWidget::on_pushButtonDelete_clicked()
{
	OC_METHODGATE();
	if(QMessageBox::Yes == QMessageBox::question(this, "Delete Actuator", "Are you sure you want to permanently delete this actuator?", QMessageBox::Yes|QMessageBox::Cancel)) {
		emit actuatorDeleted(mID);
	}
}


