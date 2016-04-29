#include "ServoInput.hpp"
#include "ui_ServoInput.h"


#include <QDebug>

quint32 ServoInput::s_sid=0;

ServoInput::ServoInput(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::ServoInput)
	, m_id(s_sid++)
	, m_low_trim(INITIAL_SERVO_TRIM)
	, m_high_trim(INITIAL_SERVO_TRIM)

{
	ui->setupUi(this);
	if(!connect(ui->numberEntryServoPosition,SIGNAL(valueChanged(int)),this,SLOT(onServoMoved()))){
		qWarning()<<"ERROR: Could not connect";
	}

	if(!connect(ui->checkBoxEnabled,SIGNAL(clicked(bool)), this,SLOT(onServoKilled()))){
		qWarning()<<"ERROR: Could not connect";
	}
	configure(m_id,m_low_trim,m_high_trim);
	ui->spinBoxHighTrim->hide();
	ui->spinBoxLowTrim->hide();
	ui->spinBoxHighTrim->setValue(m_low_trim);
	ui->spinBoxLowTrim->setValue(m_high_trim);
}

ServoInput::~ServoInput()
{
	delete ui;
}


void ServoInput::configure(quint32 id, int low_trim, int high_trim)
{
	m_low_trim=low_trim;
	m_high_trim=high_trim;
	m_id=id;
	ui->numberEntryServoPosition->setValue(1500);
	reconfigureTrim();
	ui->labelID->setText(QString("#%1: ").arg(m_id, 2, 10, QChar('0')));
}

void ServoInput::reconfigureTrim(){
	ui->numberEntryServoPosition->configure(500-m_low_trim,2500+m_high_trim,50,ui->numberEntryServoPosition->value()," µs","Servo position","servo_position");
}

void ServoInput::disableServo(){
	ui->checkBoxEnabled->setChecked(false);
}


void ServoInput::on_pushButtonCenter_clicked(){
	ui->numberEntryServoPosition->setValue((ui->numberEntryServoPosition->minimum()+ui->numberEntryServoPosition->maximum())/2);
}


void ServoInput::onServoMoved()
{
	qreal val=ui->numberEntryServoPosition->value();
	const qreal min=ui->numberEntryServoPosition->minimum();
	const qreal max=ui->numberEntryServoPosition->maximum();
	val-=min;
	val/=(max-min);
	ui->checkBoxEnabled->setChecked(true);
	emit servoMoved(m_id, val);
	//pos[0]=(val*2.0)-1.0;
	//hexy->move(pos,0b00000000000000000000000000000001);
}

void ServoInput::onServoKilled()
{
	if(!ui->checkBoxEnabled->isChecked()){
		emit servoKilled(m_id);
	}
}

void ServoInput::on_pushButton_toggled(bool checked)
{
	ui->spinBoxHighTrim->setVisible(checked);
	ui->spinBoxLowTrim->setVisible(checked);
}

void ServoInput::on_spinBoxLowTrim_valueChanged(int val)
{
	m_low_trim=val;
	reconfigureTrim();
	onServoMoved();
}


void ServoInput::on_spinBoxHighTrim_valueChanged(int val)
{
	m_high_trim=val;
	reconfigureTrim();
	onServoMoved();
}
