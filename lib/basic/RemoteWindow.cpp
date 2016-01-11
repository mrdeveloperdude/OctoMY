#include "RemoteWindow.hpp"
#include "ui_RemoteWindow.h"
#include "Remote.hpp"

#include <QDebug>
#include <QAccelerometerReading>
#include <QCompassReading>
#include <QGyroscopeReading>


RemoteWindow::RemoteWindow(Remote *remote, QWidget *parent) :
	QWidget(parent)
  , ui(new Ui::RemoteWindow)
  , remote(remote)
{
	ui->setupUi(this);
	ui->stackedWidget->setCurrentWidget(ui->pageConnect);
	if(0!=remote){
		ui->labelGPS->setText("WAITING FOR GPS");
		ui->labelCompass->setText("WAITING FOR COMPASS");
		ui->labelGyroscope->setText("WAITING FOR GYRO");
		remote->hookSignals(this);
	}
	else{
		ui->labelGPS->setText("N/A");
		ui->labelCompass->setText("N/A");
		ui->labelGyroscope->setText("N/A");
	}
	QPalette p=ui->logScroll->palette();
	p.setColor(QPalette::Base, QColor(0, 0, 0, 64));
	ui->logScroll->setPalette(p);
#ifdef Q_OS_ANDROID
	showFullScreen();
#endif
}

RemoteWindow::~RemoteWindow(){
	delete ui;
}




void RemoteWindow::onPositionUpdated(const QGeoPositionInfo &info){
	ui->labelGPS->setText("POSITION: "+info.coordinate().toString()+"@"+info.timestamp().toString());
	qDebug() << "Position updated:" << info;
}


void RemoteWindow::onCompassUpdated(QCompassReading *r){
	ui->labelCompass->setText("COMPASS: "+QString::number(r->azimuth()));
}

void RemoteWindow::onAccelerometerUpdated(QAccelerometerReading *r){
	ui->labelAccelerometer->setText("ACCEL: <"+QString::number(r->x())+", "+QString::number(r->y())+", "+ QString::number(r->z())+">");
}

void RemoteWindow::onGyroscopeUpdated(QGyroscopeReading *r){
	ui->labelGyroscope->setText("GYRO: <"+QString::number(r->x())+", "+QString::number(r->y())+", "+ QString::number(r->z())+">");
}

void RemoteWindow::on_pushButtonStart_clicked(){
	ui->stackedWidget->setCurrentWidget(ui->pageRunning);
}




void RemoteWindow::keyReleaseEvent(QKeyEvent *e){
	if(Qt::Key_Back==e->key()){
		if(ui->pageConnect==ui->stackedWidget->currentWidget()){
			qDebug()<<"EXITING APP ON BACK BUTTON";
			ui->stackedWidget->setCurrentWidget(ui->pageConfirmQuit);
		}
		else if(ui->pageRunning==ui->stackedWidget->currentWidget()){
			qDebug()<<"GOING TO CONNECTION SCREEN ON BACK BUTTON";
			ui->stackedWidget->setCurrentWidget(ui->pageConnect);
		}
		else if(ui->pageConfirmQuit==ui->stackedWidget->currentWidget()){
			qDebug()<<"GOING TO CONNECTION SCREEN ON BACK BUTTON";
			ui->stackedWidget->setCurrentWidget(ui->pageConnect);
		}
		else{
			qDebug()<<"ERROR ON BACK BUTTON";
		}
		e->accept();
	}
	else{
		qDebug()<<"UNKNOWN BUTTON: "<<e;
	}
}

void RemoteWindow::on_pushButtonConfirmQuit_clicked(){
	exit(0);
}
