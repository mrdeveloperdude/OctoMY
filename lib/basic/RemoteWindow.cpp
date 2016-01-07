#include "RemoteWindow.hpp"
#include "ui_RemoteWindow.h"
#include "Remote.hpp"

#include <QDebug>
#include <QAccelerometerReading>
#include <QCompassReading>

RemoteWindow::RemoteWindow(Remote *remote, QWidget *parent) :
	QWidget(parent)
  , ui(new Ui::RemoteWindow)
  , remote(remote)
{
	ui->setupUi(this);

	if(0!=remote){
		ui->labelGPS->setText("WAITING FOR GPS");
		ui->labelCompass->setText("WAITING FOR COMPASS");
		remote->hookSignals(this);
	}
	else{
		ui->labelGPS->setText("NO GPS FOUND");
		ui->labelCompass->setText("NO COMPASS FOUND");
	}
	//showFullScreen();
}

RemoteWindow::~RemoteWindow(){
	delete ui;
}




void RemoteWindow::onPositionUpdated(const QGeoPositionInfo &info){
	ui->labelGPS->setText("POSITION: "+info.coordinate().toString()+"@"+info.timestamp().toString());
	qDebug() << "Position updated:" << info;
}


void RemoteWindow::onCompassUpdated(QCompassReading *r){
	ui->labelCompass->setText("BEARING:: "+QString::number(r->azimuth()));
}

void RemoteWindow::onAccelerometerUpdated(QAccelerometerReading *r){
	ui->labelAccelerometer->setText("ACCELERATION:: <"+QString::number(r->x())+", "+QString::number(r->y())+", "+ QString::number(r->z())+">");
}
