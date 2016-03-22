#include "Camera.hpp"
#include "ui_Camera.h"
#include "CameraSettings.hpp"

#include <QMediaService>
#include <QMediaRecorder>
#include <QCameraViewfinder>
#include <QCameraInfo>
#include <QMediaMetaData>

#include <QMessageBox>
#include <QPalette>

#include <QtWidgets>

Q_DECLARE_METATYPE(QCameraInfo)

Camera::Camera(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::Camera)
	, camera(0)
	, imageCapture(0)
	, mediaRecorder(0)
	, videoProbe(0)
	, isCapturingImage(false)
	, applicationExiting(false)

{
	ui->setupUi(this);
	//Camera devices:
	QActionGroup *videoDevicesGroup = new QActionGroup(this);
	videoDevicesGroup->setExclusive(true);
	qDebug()<<"ADDING CAMERAS TO LIST: ";
	foreach (const QCameraInfo &cameraInfo, QCameraInfo::availableCameras()) {
		QAction *videoDeviceAction = new QAction(cameraInfo.description(), videoDevicesGroup);
		videoDeviceAction->setCheckable(true);
		videoDeviceAction->setData(QVariant::fromValue(cameraInfo));
		if (cameraInfo == QCameraInfo::defaultCamera()){
			videoDeviceAction->setChecked(true);
		}
		ui->listWidget->addAction(videoDeviceAction);
		qDebug()<<" + "<<cameraInfo.description();
	}
	connect(videoDevicesGroup, SIGNAL(triggered(QAction*)), SLOT(updateCameraDevice(QAction*)));
	connect(ui->captureWidget, SIGNAL(currentChanged(int)), SLOT(updateCaptureMode()));
	ui->pushButtonToggleCamera->setChecked(true);
	setCamera(QCameraInfo::defaultCamera());
}

Camera::~Camera()
{
	delete mediaRecorder;
	delete imageCapture;
	delete camera;
}

void Camera::setCamera(const QCameraInfo &cameraInfo)
{

	{
		delete camera;
		camera = new QCamera(cameraInfo);
		if(!connect(camera, SIGNAL(stateChanged(QCamera::State)), this, SLOT(updateCameraState(QCamera::State)))){
			qWarning()<<"ERROR: could not connect";
		}
		if(!connect(camera, SIGNAL(error(QCamera::Error)), this, SLOT(displayCameraError()))){
			qWarning()<<"ERROR: could not connect";
		}
	}

	{
		delete mediaRecorder;
		mediaRecorder = new QMediaRecorder(camera);
		if(!connect(mediaRecorder, SIGNAL(stateChanged(QMediaRecorder::State)), this, SLOT(updateRecorderState(QMediaRecorder::State)))){
			qWarning()<<"ERROR: could not connect";
		}
	}

	{
		delete imageCapture;
		imageCapture = new QCameraImageCapture(camera);
		if(!connect(mediaRecorder, SIGNAL(durationChanged(qint64)), this, SLOT(updateRecordTime()))){
			qWarning()<<"ERROR: could not connect";
		}
		if(!connect(mediaRecorder, SIGNAL(error(QMediaRecorder::Error)), this, SLOT(displayRecorderError()))){
			qWarning()<<"ERROR: could not connect";
		}
	}
	mediaRecorder->setMetaData(QMediaMetaData::Title, QVariant(QLatin1String("OctoMY™")));
	if(!connect(ui->exposureCompensation, SIGNAL(valueChanged(int)), SLOT(setExposureCompensation(int)))){
		qWarning()<<"ERROR: could not connect";
	}
	camera->setViewfinder(ui->viewfinder);


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

	updateCameraState(camera->state());
	updateLockStatus(camera->lockStatus(), QCamera::UserRequest);
	updateRecorderState(mediaRecorder->state());

	if(!connect(imageCapture, SIGNAL(readyForCaptureChanged(bool)), this, SLOT(readyForCapture(bool)))){
		qWarning()<<"ERROR: could not connect";
	}
	if(!connect(imageCapture, SIGNAL(imageCaptured(int,QImage)), this, SLOT(processCapturedImage(int,QImage)))){
		qWarning()<<"ERROR: could not connect";
	}
	if(!connect(imageCapture, SIGNAL(imageSaved(int,QString)), this, SLOT(imageSaved(int,QString)))){
		qWarning()<<"ERROR: could not connect";
	}
	if(!connect(imageCapture, SIGNAL(error(int,QCameraImageCapture::Error,QString)), this, SLOT(displayCaptureError(int,QCameraImageCapture::Error,QString)))){
		qWarning()<<"ERROR: could not connect";
	}
	if(!connect(camera, SIGNAL(lockStatusChanged(QCamera::LockStatus,QCamera::LockChangeReason)), this, SLOT(updateLockStatus(QCamera::LockStatus,QCamera::LockChangeReason)))){
		qWarning()<<"ERROR: could not connect";
	}
	ui->captureWidget->setTabEnabled(0, (camera->isCaptureModeSupported(QCamera::CaptureStillImage)));
	ui->captureWidget->setTabEnabled(1, (camera->isCaptureModeSupported(QCamera::CaptureVideo)));
	updateCaptureMode();
	camera->start();
}

void Camera::keyPressEvent(QKeyEvent * event)
{
	if (event->isAutoRepeat())
		return;

	switch (event->key()) {
		case Qt::Key_CameraFocus:
			displayViewfinder();
			camera->searchAndLock();
			event->accept();
			break;
		case Qt::Key_Camera:
			if (camera->captureMode() == QCamera::CaptureStillImage) {
				takeImage();
			} else {
				if (mediaRecorder->state() == QMediaRecorder::RecordingState)
					stop();
				else
					record();
			}
			event->accept();
			break;
		default:
			QWidget::keyPressEvent(event);
	}
}

void Camera::keyReleaseEvent(QKeyEvent *event)
{
	if (event->isAutoRepeat())
		return;

	switch (event->key()) {
		case Qt::Key_CameraFocus:
			camera->unlock();
			break;
		default:
			QWidget::keyReleaseEvent(event);
	}
}

void Camera::updateRecordTime()
{
	QString str = QString("Recorded %1 sec").arg(mediaRecorder->duration()/1000);
	showMessage(str);
}

void Camera::processCapturedImage(int requestId, const QImage& img)
{
	Q_UNUSED(requestId);
	QImage scaledImage = img.scaled(ui->viewfinder->size(),
									Qt::KeepAspectRatio,
									Qt::SmoothTransformation);

	ui->lastImagePreviewLabel->setPixmap(QPixmap::fromImage(scaledImage));

	// Display captured image for 4 seconds.
	displayCapturedImage();
	QTimer::singleShot(4000, this, SLOT(displayViewfinder()));
}



void Camera::configureCaptureSettings()
{
	CameraSettings settingsDialog(mediaRecorder, imageCapture);

	settingsDialog.setAudioSettings(audioSettings);
	settingsDialog.setVideoSettings(videoSettings);
	settingsDialog.setFormat(videoContainerFormat);

	if (settingsDialog.exec()) {
		audioSettings = settingsDialog.audioSettings();
		videoSettings = settingsDialog.videoSettings();
		videoContainerFormat = settingsDialog.format();

		mediaRecorder->setEncodingSettings(
					audioSettings,
					videoSettings,
					videoContainerFormat);

		imageSettings = settingsDialog.imageSettings();
		imageCapture->setEncodingSettings(imageSettings);
	}
}


void Camera::record()
{
	mediaRecorder->record();
	updateRecordTime();
}

void Camera::pause()
{
	mediaRecorder->pause();
}

void Camera::stop()
{
	mediaRecorder->stop();
}

void Camera::setMuted(bool muted)
{
	mediaRecorder->setMuted(muted);
}

void Camera::toggleLock()
{
	switch (camera->lockStatus()) {
		case QCamera::Searching:
		case QCamera::Locked:
			camera->unlock();
			break;
		case QCamera::Unlocked:
			camera->searchAndLock();
	}
}

void Camera::updateLockStatus(QCamera::LockStatus status, QCamera::LockChangeReason reason)
{
	QColor indicationColor = Qt::black;

	switch (status) {
		case QCamera::Searching:
			indicationColor = Qt::yellow;
			showMessage(tr("Focusing..."));
			ui->lockButton->setText(tr("Focusing..."));
			break;
		case QCamera::Locked:
			indicationColor = Qt::darkGreen;
			ui->lockButton->setText(tr("Unlock"));
			showMessage(tr("Focused"));
			break;
		case QCamera::Unlocked:
			indicationColor = reason == QCamera::LockFailed ? Qt::red : Qt::black;
			ui->lockButton->setText(tr("Focus"));
			if (reason == QCamera::LockFailed){
				showMessage(tr("Focus Failed"));
			}
	}

	QPalette palette = ui->lockButton->palette();
	palette.setColor(QPalette::ButtonText, indicationColor);
	ui->lockButton->setPalette(palette);
}

void Camera::takeImage()
{
	isCapturingImage = true;
	imageCapture->capture();
}

void Camera::displayCaptureError(int id, const QCameraImageCapture::Error error, const QString &errorString)
{
	Q_UNUSED(id);
	Q_UNUSED(error);
	QMessageBox::warning(this, tr("Image Capture Error"), errorString);
	isCapturingImage = false;
}

void Camera::startCamera()
{
	ui->pushButtonToggleCamera->setEnabled(false);
	ui->pushButtonToggleCamera->setText(ui->pushButtonToggleCamera->isChecked()?"Stop Camera":"Start Camera");
	camera->start();
	ui->viewfinder->show();
	ui->pushButtonToggleCamera->setEnabled(true);
}

void Camera::stopCamera()
{
	ui->pushButtonToggleCamera->setEnabled(false);
	ui->pushButtonToggleCamera->setText(ui->pushButtonToggleCamera->isChecked()?"Stop Camera":"Start Camera");
	ui->viewfinder->hide();
	camera->stop();
	ui->pushButtonToggleCamera->setEnabled(true);
}

void Camera::updateCaptureMode()
{
	int tabIndex = ui->captureWidget->currentIndex();
	QCamera::CaptureModes captureMode = tabIndex == 0 ? QCamera::CaptureStillImage : QCamera::CaptureVideo;

	if (camera->isCaptureModeSupported(captureMode))
		camera->setCaptureMode(captureMode);
}

void Camera::updateCameraState(QCamera::State state)
{
	switch (state) {
		case QCamera::ActiveState:
			ui->captureWidget->setEnabled(true);
			ui->pushButtonSettings->setEnabled(true);
			break;
		case QCamera::UnloadedState:
		case QCamera::LoadedState:
			ui->captureWidget->setEnabled(false);
			ui->pushButtonSettings->setEnabled(false);
	}
}

void Camera::updateRecorderState(QMediaRecorder::State state)
{
	switch (state) {
		case QMediaRecorder::StoppedState:
			ui->recordButton->setEnabled(true);
			ui->pauseButton->setEnabled(true);
			ui->stopButton->setEnabled(false);
			break;
		case QMediaRecorder::PausedState:
			ui->recordButton->setEnabled(true);
			ui->pauseButton->setEnabled(false);
			ui->stopButton->setEnabled(true);
			break;
		case QMediaRecorder::RecordingState:
			ui->recordButton->setEnabled(false);
			ui->pauseButton->setEnabled(true);
			ui->stopButton->setEnabled(true);
			break;
	}
}

void Camera::setExposureCompensation(int index)
{
	camera->exposure()->setExposureCompensation(index*0.5);
}

void Camera::displayRecorderError()
{
	//QMessageBox::warning(this, tr("Capture error"), mediaRecorder->errorString());
	qWarning()<<"WARNING: Capture error :"<<mediaRecorder->errorString();
}

void Camera::displayCameraError()
{
	//QMessageBox::warning(this, tr("Camera error"), camera->errorString());
	qWarning()<<"WARNING: Camera error :"<<camera->errorString();
}

void Camera::updateCameraDevice(QAction *action)
{
	setCamera(qvariant_cast<QCameraInfo>(action->data()));
}

void Camera::displayViewfinder()
{
	ui->stackedWidget->setCurrentIndex(0);
}

void Camera::displayCapturedImage()
{
	ui->stackedWidget->setCurrentIndex(1);
}

void Camera::readyForCapture(bool ready)
{
	ui->takeImageButton->setEnabled(ready);
}

void Camera::imageSaved(int id, const QString &fileName)
{
	Q_UNUSED(id);
	Q_UNUSED(fileName);

	isCapturingImage = false;
	if (applicationExiting)
		close();
}

void Camera::closeEvent(QCloseEvent *event)
{
	if (isCapturingImage) {
		setEnabled(false);
		applicationExiting = true;
		event->ignore();
	} else {
		event->accept();
	}
}

void Camera::on_pushButtonToggleCamera_clicked()
{
	ui->pushButtonToggleCamera->isChecked()?startCamera():stopCamera();
}

void Camera::showMessage(QString msg){
	ui->labelMessage->setText(msg);
}

void Camera::on_pushButtonSettings_clicked()
{
	configureCaptureSettings();
}



void Camera::detectBarcodes(const QVideoFrame &frame){
	qDebug()<<"GOT FRAME "<<frame;
}
