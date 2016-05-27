#include "RealtimeValuesWidget.hpp"
#include "ui_RealtimeValuesWidget.h"

#include "agent/Agent.hpp"


#include <QAccelerometerReading>
#include <QCompassReading>
#include <QGyroscopeReading>
#include <QGeoPositionInfo>


RealtimeValuesWidget::RealtimeValuesWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::RealtimeValuesWidget)
{
	ui->setupUi(this);


	Agent *agent=nullptr;

	if(nullptr!=agent){
		ui->labelLocal->setText("WAITING FOR LOCAL");
		ui->labelHub->setText("WAITING FOR HUB");
		ui->labelGPS->setText("WAITING FOR GPS");
		ui->labelCompass->setText("WAITING FOR COMPASS");
		ui->labelGyroscope->setText("WAITING FOR GYRO");
		ui->labelAccelerometer->setText("WAITING FOR ACCELEROMETER");
		agent->hookSignals(*this);
	}
	else{
		ui->labelLocal->setText("N/A");
		ui->labelHub->setText("N/A");
		ui->labelGPS->setText("N/A");
		ui->labelCompass->setText("N/A");
		ui->labelGyroscope->setText("N/A");
		ui->labelAccelerometer->setText("N/A");
	}
}

RealtimeValuesWidget::~RealtimeValuesWidget()
{
	delete ui;
}



void RealtimeValuesWidget::onPositionUpdated(const QGeoPositionInfo &info){
	ui->labelGPS->setText("GPS: "+QString::number(info.coordinate().latitude())+", "+QString::number(info.coordinate().longitude()));
	//appendLog("GPS update: "+QString::number(info.coordinate().latitude())+", "+QString::number(info.coordinate().longitude()));
}


void RealtimeValuesWidget::onCompassUpdated(QCompassReading *r){
	ui->labelCompass->setText("COMPASS: "+QString::number(r->azimuth()));
}

void RealtimeValuesWidget::onAccelerometerUpdated(QAccelerometerReading *r){
	ui->labelAccelerometer->setText("ACCEL: <"+QString::number(r->x())+", "+QString::number(r->y())+", "+ QString::number(r->z())+">");
}

void RealtimeValuesWidget::onGyroscopeUpdated(QGyroscopeReading *r){
	ui->labelGyroscope->setText("GYRO: <"+QString::number(r->x())+", "+QString::number(r->y())+", "+ QString::number(r->z())+">");
}

