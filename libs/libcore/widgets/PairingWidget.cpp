#include "PairingWidget.hpp"
#include "ui_PairingWidget.h"

#include "../libzbar/ZBarScanner.hpp"
#include "camera/PoorMansProbe.hpp"
#include "security/KeyStore.hpp"

#include "basic/UniquePlatformFingerprint.hpp"


#include <QDateTime>
#include <QDebug>
#include <QCamera>
#include <QCameraInfo>
#include <QVideoWidget>

#include <QVideoProbe>
#include <QVideoFrame>


const qint64 PairingWidget::TIMEOUT=30000;



QCameraInfo getBestCamera(){
	//Try to get front facing camera because it serves this purpose best
	auto list=QCameraInfo::availableCameras(QCamera::FrontFace);
	QCameraInfo ci;
	if(list.count()>0){
		ci=list.at(0);
	}
	//Fall back to any old camera
	if(ci.isNull()){
		ci=QCameraInfo::defaultCamera();
	}
	return ci;
}

PairingWidget::PairingWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::PairingWidget)
	, badgeDisableTime(0)
	, scanDisableTime(0)
	, camera(0)
	, videoProbe(0)
	, poorVideoProbe(0)
	, zbar(0)
	, viewfinder(0)
	, keystore(KeyStore::getInstance())
{
	ui->setupUi(this);
	ui->widgetQR->setVisible(false);
	ui->widgetIdenticon->setVisible(false);
	ui->widgetCameraViewfinder->setVisible(false);
	ui->listWidgetCandidates->setVisible(false);
	countDownTimer.setTimerType(Qt::PreciseTimer);
	countDownTimer.setInterval(1000);
	if(!connect(&countDownTimer,SIGNAL(timeout()),this,SLOT(onCountDownTimeout()))){
		qWarning()<<"ERROR: Could not connect";
	}
	if(getBestCamera().isNull()){
		ui->pushButtonScanner->setEnabled(false);
		ui->pushButtonScanner->setText("No cameras available for scanning");
	}

}

PairingWidget::~PairingWidget()
{
	delete ui;
}



PortableID PairingWidget::getMyPortableID()
{
	PortableID out;
	out.tier="remote";
	out.id=UniquePlatformFingerprint::getInstance().platform().getHEX();
	out.name="granny";
	QString pub=keystore.getLocalPublicKey();
	out.pubkey=pub.mid(27,pub.length()-27-26).replace(QRegExp("\n"), "");
	return out;
}



void PairingWidget::on_pushButtonBadge_toggled(bool show)
{
	if(show && !keystore.isReady()){
		qDebug()<<"Cannot show badge, keystore not ready";
		ui->pushButtonBadge->setChecked(false);
		return;
	}
	ui->widgetQR->setVisible(show);
	ui->widgetQR->setQRData(getMyPortableID().toString());
	ui->widgetIdenticon->setVisible(show);
	ui->pushButtonScanner->setVisible(!show);
	qDebug()<<"Badge "<<(show?"on":"off");
	if(show){
		badgeDisableTime=TIMEOUT+QDateTime::currentMSecsSinceEpoch();
		onCountDownTimeout();
		if(!countDownTimer.isActive()){
			countDownTimer.start();
		}
	}
	else{
		ui->pushButtonBadge->setText("Show Badge");
		badgeDisableTime=0;
	}
}


void PairingWidget::onKeystoreReady()
{

}

void PairingWidget::detectBarcodes(const QVideoFrame &frame)
{
	if(0==zbar){
		zbar=new ZBarScanner;
	}
	if(0!=zbar){
		zbar->scan(frame);
	}
}

void PairingWidget::cameraStatusUpdated(QCamera::Status s){
	if(QCamera::ActiveStatus==s){
		ui->widgetSpinnerScanner->stop();
		ui->widgetCameraViewfinder->show();
	}
	else{
		ui->widgetSpinnerScanner->start();
		ui->widgetCameraViewfinder->hide();
	}
}


void PairingWidget::on_pushButtonScanner_toggled(bool show)
{
	qDebug()<<"Scan "<<(show?"on":"off");
	ui->widgetCameraViewfinder->setVisible(show);
	ui->pushButtonBadge->setVisible(!show);
	if(show){
		ui->widgetCameraViewfinder->hide();
		ui->widgetSpinnerScanner->start();
		QCameraInfo ci=getBestCamera();
		qDebug()<<"TRYING TO START CAMERA "<<ci;
		if(!ci.isNull()){
			camera = new QCamera(ci);
			if(0!=camera){

				if(!connect(camera,SIGNAL(statusChanged(QCamera::Status)),this,SLOT(cameraStatusUpdated(QCamera::Status)))){
					qWarning()<<"ERROR: Could not connect";
				}

				QCameraViewfinderSettings viewfinderSettings;
				viewfinderSettings.setResolution(640, 480);
				viewfinderSettings.setMinimumFrameRate(0.0);
				viewfinderSettings.setMaximumFrameRate(30.0);
				camera->setViewfinderSettings(viewfinderSettings);
				/*
			if(0==viewfinder){
				viewfinder=new QVideoWidget();
				delete ui->widgetCameraViewfinder->layout();
				QGridLayout *gridLayout = new QGridLayout(ui->widgetCameraViewfinder);
				gridLayout->addWidget(viewfinder, 0, 0);
			}
			*/
				camera->setViewfinder(ui->widgetCameraViewfinder);


				//TODO: Handles iOS
#ifdef Q_OS_ANDROID
				if(0!=videoProbe){
					videoProbe->deleteLater();
					videoProbe=0;
				}
				videoProbe = new QVideoProbe(this);
				if (videoProbe->setSource(camera)) {
					// Probing succeeded, videoProbe->isValid() should be true.
					if(!connect(videoProbe, SIGNAL(videoFrameProbed(QVideoFrame)), this, SLOT(detectBarcodes(QVideoFrame)))){
						qWarning()<<"ERROR: could not connect";
					}
				}
				else{
					qWarning()<<"ERROR: Could not set up probing of camera";
				}
#else

				/*
			if(0!=poorVideoProbe){
				poorVideoProbe->deleteLater();
				poorVideoProbe=0;
			}
			poorVideoProbe=new PoorMansProbe(this);
			if (poorVideoProbe->setSource(camera)) {
				// Probing succeeded, videoProbe->isValid() should be true.
				if(!connect(poorVideoProbe, SIGNAL(videoFrameProbed(QVideoFrame)), this, SLOT(detectBarcodes(QVideoFrame)))){
					qWarning()<<"ERROR: could not connect";
				}
			}
			else{
				qWarning()<<"ERROR: Could not set up probing of camera";
			}
			*/
#endif
				//Invoke later to make sure ui update does not lagg
				QTimer::singleShot(0, camera, SLOT(start()));
			}
			scanDisableTime=TIMEOUT+QDateTime::currentMSecsSinceEpoch();
			onCountDownTimeout();
			if(!countDownTimer.isActive()){
				countDownTimer.start();
			}
		}
	}
	else{
		delete camera;
		camera=0;
		ui->pushButtonScanner->setText("Start Scanner");
		scanDisableTime=0;
	}
}

void PairingWidget::onCountDownTimeout()
{
	const qint64 now=QDateTime::currentMSecsSinceEpoch();
	const qint64 badgeLeft=(badgeDisableTime-now)/1000;
	const qint64 scanLeft=(scanDisableTime-now)/1000;
	if(badgeDisableTime>0){
		if(badgeLeft>0){
			ui->pushButtonBadge->setText("Hide Badge ("+QString::number(badgeLeft)+" sec)");
		}
		else{
			ui->pushButtonBadge->setChecked(false);
		}
	}

	if(scanDisableTime>0){
		if(scanLeft>0){
			ui->pushButtonScanner->setText("Stop Scanner ("+QString::number(scanLeft)+" sec)");
		}
		else{
			ui->pushButtonScanner->setChecked(false);
		}
	}

	if(badgeDisableTime<=0 && scanDisableTime <=0){
		qDebug()<<"timer stopping..";
		countDownTimer.stop();
	}
}
