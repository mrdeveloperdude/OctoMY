#include "HexySerial.hpp"

#include "SerialSettingsDialog.hpp"


#include <QDebug>
#include <QtGlobal>
#include <QStringBuilder>


/*
HW Descriptor for Arduino

Actuator{
enum motor_type{step, dc}
int gear_ratio_numerator, gear_ratio_denominator;
int encoder_steps;
bool potensiometer
bool tachometer
bool encoder;
bool endstop_a;
bool endstop_b;

}

*/

HexySerial::HexySerial(QObject *parent)
	: QObject(parent)
	, lastPos{0}
{
	serial = new QSerialPort(this);
	settings = new SerialSettingsDialog;

	connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)));
	connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
	connect(settings,SIGNAL(settingsChanged()), this, SLOT(onSettingsChanged()));
}

HexySerial::~HexySerial(){
	//Limp servos to avoid frying them if they are trying to reach impossible positions
	kill();

	delete settings;
}


void HexySerial::openSerialPort(){
	SerialSettingsDialog::Settings p = settings->settings();
	serial->setPortName(p.name);
	serial->setBaudRate(p.baudRate);
	serial->setDataBits(p.dataBits);
	serial->setParity(p.parity);
	serial->setStopBits(p.stopBits);
	serial->setFlowControl(p.flowControl);
	if (serial->open(QIODevice::ReadWrite)) {
		qDebug()<<tr("Connected to %1 : %2, %3, %4, %5, %6").arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits).arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl);
	} else {
		qDebug()<<"ERROR OPENING: "<<serial->errorString();
	}
}

void HexySerial::closeSerialPort(){
	if (serial->isOpen()){
		serial->close();
	}
	qDebug()<<"Disconnected";
}


void HexySerial::writeData(const QByteArray &data){
	//qDebug()<<"OUT: "<<data;
	serial->write(data);
}

void HexySerial::readData(){
	QByteArray data = serial->readAll();
	qDebug()<<data;
	//TODO:parse and handle data
}

void HexySerial::handleError(QSerialPort::SerialPortError error){
	if (error == QSerialPort::ResourceError) {
		qDebug()<<"Critical Error"<<serial->errorString();
		closeSerialPort();
	}
}

void HexySerial::onSettingsChanged(){
	closeSerialPort();
	openSerialPort();
	//version();
	writeData("\n");
	emit settingsChanged();
}


void HexySerial::configure(){
	settings->show();
}


bool HexySerial::isConnected(){
	return serial->isOpen();
}

///// Reference: http://arcbotics.com/lessons/servotor32-commands/


void HexySerial::kill(quint32 flags){
	//Trivial reject: kill ALL
	if(0xFFFFFFFF==flags){
		writeData("K\n");
	}
	else{
		QString data;
		for(quint32 i=0;i<SERVO_COUNT;++i){
			if((flags & (0x1<<i))>0){
				data += QLatin1Literal("#") +  QString::number(i) + QLatin1String("L\n");
			}
		}
		writeData(data.toLatin1());
	}
}


void HexySerial::center(){
	writeData("C\n");
}


void HexySerial::version(){
	writeData("V\n");
}


void HexySerial::debug(){
	writeData("D\n");
}


inline qreal fclamp(qreal x, qreal a, qreal b){
	return x < a ? a : (x > b ? b : x);
}


//TODO: look at binary extension introdyuced in 2.1 version of hexy firmware to improve performance
void HexySerial::move(qreal pos[], quint32 flags){
	QString data;
	quint32 j=0;
	for(quint32 i=0;i<SERVO_COUNT;++i){
		if((flags & (0x1<<i))>0){
			const quint32 p=(quint32)(fclamp(pos[i],-1.0,1.0)*1000.0+1500.0);
			//Skip unecessary communication if value did not change
			if(lastPos[i]!=p){
				lastPos[i]=p;
				data += "#" +QString::number(i) + "P" + QString::number(p) +"\n";
				if(0==i) {
					//qDebug()<<data;
				}
			}
			j++;
		}
	}
	writeData(data.toLatin1());
}
