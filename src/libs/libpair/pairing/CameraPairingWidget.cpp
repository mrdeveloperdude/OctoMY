#include "CameraPairingWidget.hpp"
#include "ui_CameraPairingWidget.h"

#include "zbar/ZBarScanner.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "uptime/ConnectionType.hpp"
#include "utility/time/HumanTime.hpp"

#include <QDateTime>
#include <QDebug>
#include <QCamera>
//#include <QCameraInfo>
#include <QVideoWidget>

//#include <QVideoProbe>
#include <QVideoFrame>

const qint64 CameraPairingWidget::TIMEOUT=30000;


// TODO: Rework this with new qmultimedia primitives for Qt6

/*
static QCameraInfo getBestCamera()
{
	OC_FUNCTIONGATE();
	//Try to get front facing camera because it serves this purpose best
	auto list=QCameraInfo::availableCameras(QCamera::FrontFace);
	QCameraInfo ci;
	if(list.count()>0) {
		ci=list.at(0);
	}
	//Fall back to any old camera
	if(ci.isNull()) {
		ci=QCameraInfo::defaultCamera();
	}
	return ci;
}
*/


CameraPairingWidget::CameraPairingWidget(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::CameraPairingWidget)
	, badgeDisableTime(0)
	, scanDisableTime(0)
//	, mCamera(nullptr)
//	, mVideoProbe(nullptr)
//	, mPoorVideoProbe(nullptr)
	, mZbar(nullptr)
	, mViewfinder(nullptr)

{
	OC_METHODGATE();
	ui->setupUi(this);
	ui->widgetQR->setVisible(false);
	ui->widgetIdenticon->setVisible(false);
	ui->widgetCameraViewfinder->setVisible(false);
	ui->listWidgetCandidates->setVisible(false);
	countDownTimer.setTimerType(Qt::PreciseTimer);
	countDownTimer.setInterval(1000);
	if(!connect(&countDownTimer,SIGNAL(timeout()),this,SLOT(onCountDownTimeout()), OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
	// TODO: Rework this with new qmultimedia primitives for Qt6
	/*
	if(getBestCamera().isNull()) {
		ui->pushButtonScanner->setEnabled(false);
		ui->pushButtonScanner->setText("No cameras available for scanning");
	}
	*/

}

CameraPairingWidget::~CameraPairingWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;

	// TODO: Rework this with new qmultimedia primitives for Qt6
	/*
	delete mCamera;
	mCamera=(nullptr);
	delete mVideoProbe;
	mVideoProbe=(nullptr);
	delete mPoorVideoProbe;
	mPoorVideoProbe=(nullptr);
	*/
	delete mZbar;
	mZbar=(nullptr);
	delete mViewfinder;
	mViewfinder=(nullptr);
}



PortableID CameraPairingWidget::getMyPortableID()
{
	OC_METHODGATE();
	PortableID out;
//	out.tier="remote";
//	out.id=UniquePlatformFingerprint::getInstance().platform().getHEX();
	//out.name="granny";
	//TODO:Reintegrate with keystore somehow
	QString pub;//=keystore.getLocalPublicKey();
	//out.pubkey=pub.mid(27,pub.length()-27-26).replace(QRegExp("\n"), "");
	return out;
}



void CameraPairingWidget::on_pushButtonBadge_toggled(bool show)
{
	OC_METHODGATE();
	/*
	 * //TODO:Reintegrate with keystore somehow
	if(show && !keystore.isReady()){
		qDebug()<<"Cannot show badge, keystore not ready";
		ui->pushButtonBadge->setChecked(false);
		return;
	}
	*/
	ui->widgetQR->setVisible(show);
	ui->widgetQR->setQRData(getMyPortableID().toPortableString());
	ui->widgetIdenticon->setVisible(show);
	ui->pushButtonScanner->setVisible(!show);
	qDebug()<<"Badge "<<(show?"on":"off");
	if(show) {
		badgeDisableTime=TIMEOUT+utility::time::currentMsecsSinceEpoch<qint64>();
		onCountDownTimeout();
		if(!countDownTimer.isActive()) {
			countDownTimer.start();
		}
	} else {
		ui->pushButtonBadge->setText("Show Badge");
		badgeDisableTime=0;
	}
}


void CameraPairingWidget::onKeystoreReady(bool ok)
{
	OC_METHODGATE();
	qDebug()<<"Keystore ready: "<<ok;
}

void CameraPairingWidget::detectBarcodes(const QVideoFrame &frame)
{
	OC_METHODGATE();
	if(nullptr==mZbar) {
		mZbar=OC_NEW ZBarScanner;
	}
	if(nullptr!=mZbar) {
		mZbar->scan(frame);
	}
}


// TODO: Rework this with new qmultimedia primitives for Qt6
/*
void CameraPairingWidget::cameraStatusUpdated(QCamera::Status s)
{
	OC_METHODGATE();
	if(QCamera::ActiveStatus==s) {
		ui->widgetSpinnerScanner->stop();
		ui->widgetCameraViewfinder->show();
	} else {
		ui->widgetSpinnerScanner->start();
		ui->widgetCameraViewfinder->hide();
	}
}
*/

void CameraPairingWidget::on_pushButtonScanner_toggled(bool show)
{
	OC_METHODGATE();
	qDebug()<<"Scan "<<(show?"on":"off");
	ui->widgetCameraViewfinder->setVisible(show);
	ui->pushButtonBadge->setVisible(!show);
	
#ifdef QT5_STUFF
	if(show) {
		ui->widgetCameraViewfinder->hide();
		ui->widgetSpinnerScanner->start();
		QCameraInfo ci=getBestCamera();
		qDebug()<<"TRYING TO START CAMERA "<<ci;
		if(!ci.isNull()) {
			mCamera = OC_NEW QCamera(ci);
			if(nullptr!=mCamera) {

				if(!connect(mCamera,SIGNAL(statusChanged(QCamera::Status)),this,SLOT(cameraStatusUpdated(QCamera::Status)))) {
					qWarning()<<"ERROR: Could not connect";
				}

				QCameraViewfinderSettings viewfinderSettings;
				viewfinderSettings.setResolution(640, 480);
				viewfinderSettings.setMinimumFrameRate(0.0);
				viewfinderSettings.setMaximumFrameRate(30.0);
				mCamera->setViewfinderSettings(viewfinderSettings);
				/*
				if(0==viewfinder){
				viewfinder=OC_NEW QVideoWidget();
				delete ui->widgetCameraViewfinder->layout();
				QGridLayout *gridLayout = OC_NEW QGridLayout(ui->widgetCameraViewfinder);
				gridLayout->addWidget(viewfinder, 0, 0);
				}
				*/
				mCamera->setViewfinder(ui->widgetCameraViewfinder);


				//TODO: Handles iOS
#ifdef Q_OS_ANDROID
				if(nullptr!=mVideoProbe) {
					mVideoProbe->deleteLater();
					mVideoProbe=nullptr;
				}
				mVideoProbe = OC_NEW QVideoProbe(this);
				if (mVideoProbe->setSource(mCamera)) {
					// Probing succeeded, videoProbe->isValid() should be true.
					if(!connect(mVideoProbe, SIGNAL(videoFrameProbed(QVideoFrame)), this, SLOT(detectBarcodes(QVideoFrame)))) {
						qWarning()<<"ERROR: could not connect";
					}
				} else {
					qWarning()<<"ERROR: Could not set up probing of camera";
				}
#else

				/*
				if(nullptr!=mPoorVideoProbe){
				mPoorVideoProbe->deleteLater();
				mPoorVideoProbe=nullptr;
				}
				mPoorVideoProbe=OC_NEW PoorMansProbe(this);
				if (mPoorVideoProbe->setSource(mCamera)) {
				// Probing succeeded, mPoorVideoProbe->isValid() should be true.
				if(!connect(mPoorVideoProbe, SIGNAL(videoFrameProbed(QVideoFrame)), this, SLOT(detectBarcodes(QVideoFrame)))){
					qWarning()<<"ERROR: could not connect";
				}
				}
				else{
				qWarning()<<"ERROR: Could not set up probing of camera";
				}
				*/
#endif
				//Invoke later to make sure ui update does not lagg
				QTimer::singleShot(0, mCamera, SLOT(start()));
			}
			scanDisableTime=TIMEOUT+utility::time::currentMsecsSinceEpoch<qint64>();
			onCountDownTimeout();
			if(!countDownTimer.isActive()) {
				countDownTimer.start();
			}
		}
	} else {
		delete mCamera;
		mCamera=nullptr;
		ui->pushButtonScanner->setText("Start Scanner");
		scanDisableTime=0;
	}
#endif
}

void CameraPairingWidget::onCountDownTimeout()
{
	OC_METHODGATE();
	const qint64 now=utility::time::currentMsecsSinceEpoch<qint64>();
	const qint64 badgeLeft=(badgeDisableTime-now)/1000;
	const qint64 scanLeft=(scanDisableTime-now)/1000;
	if(badgeDisableTime>0) {
		if(badgeLeft>0) {
			ui->pushButtonBadge->setText("Hide Badge ("+QString::number(badgeLeft)+" sec)");
		} else {
			ui->pushButtonBadge->setChecked(false);
		}
	}

	if(scanDisableTime>0) {
		if(scanLeft>0) {
			ui->pushButtonScanner->setText("Stop Scanner ("+QString::number(scanLeft)+" sec)");
		} else {
			ui->pushButtonScanner->setChecked(false);
		}
	}

	if(badgeDisableTime<=0 && scanDisableTime <=0) {
		qDebug()<<"timer stopping..";
		countDownTimer.stop();
	}
}
