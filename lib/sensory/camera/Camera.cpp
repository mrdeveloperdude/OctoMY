#include "Camera.hpp"

#include <QCamera>
#include <QCameraImageCapture>
#include <QMediaRecorder>
#include <QMessageBox>
#include <QActionGroup>
#include <QMediaMetaData>
#include <QTimer>

Camera::Camera(QObject *parent) : QObject(parent)
  , camera(0)
  , imageCapture(0)
  , mediaRecorder(0)
  , isCapturingImage(false)
  , applicationExiting(false)
{
	foreach (const QCameraInfo &cameraInfo, QCameraInfo::availableCameras()) {
		//TODO:Put them here
	}
	setCamera(QCameraInfo::defaultCamera());
}

Camera::~Camera(){
	delete mediaRecorder;
	delete imageCapture;
	delete camera;
}

void Camera::setCamera(const QCameraInfo &cameraInfo){
	delete imageCapture;
	delete mediaRecorder;
	delete camera;

	camera = new QCamera(cameraInfo);

	connect(camera, SIGNAL(stateChanged(QCamera::State)), this, SLOT(updateCameraState(QCamera::State)));
	connect(camera, SIGNAL(error(QCamera::Error)), this, SLOT(displayCameraError()));

	mediaRecorder = new QMediaRecorder(camera);
	connect(mediaRecorder, SIGNAL(stateChanged(QMediaRecorder::State)), this, SLOT(updateRecorderState(QMediaRecorder::State)));

	imageCapture = new QCameraImageCapture(camera);

	connect(mediaRecorder, SIGNAL(durationChanged(qint64)), this, SLOT(updateRecordTime()));
	connect(mediaRecorder, SIGNAL(error(QMediaRecorder::Error)), this, SLOT(displayRecorderError()));

	mediaRecorder->setMetaData(QMediaMetaData::Title, QVariant(QLatin1String("Test Title")));

//	camera->setViewfinder(ui->viewfinder);

	updateCameraState(camera->state());
	updateLockStatus(camera->lockStatus(), QCamera::UserRequest);
	updateRecorderState(mediaRecorder->state());

	connect(imageCapture, SIGNAL(readyForCaptureChanged(bool)), this, SLOT(readyForCapture(bool)));
	connect(imageCapture, SIGNAL(imageCaptured(int,QImage)), this, SLOT(processCapturedImage(int,QImage)));
	connect(imageCapture, SIGNAL(imageSaved(int,QString)), this, SLOT(imageSaved(int,QString)));
	connect(imageCapture, SIGNAL(error(int,QCameraImageCapture::Error,QString)), this,
			SLOT(displayCaptureError(int,QCameraImageCapture::Error,QString)));

	connect(camera, SIGNAL(lockStatusChanged(QCamera::LockStatus,QCamera::LockChangeReason)),
			this, SLOT(updateLockStatus(QCamera::LockStatus,QCamera::LockChangeReason)));

//	ui->captureWidget->setTabEnabled(0, (camera->isCaptureModeSupported(QCamera::CaptureStillImage)));
	//ui->captureWidget->setTabEnabled(1, (camera->isCaptureModeSupported(QCamera::CaptureVideo)));

	updateCaptureMode();
	camera->start();
}

void Camera::updateRecordTime(){
	QString str = QString("Recorded %1 sec").arg(mediaRecorder->duration()/1000);
//	ui->statusbar->showMessage(str);
}

void Camera::processCapturedImage(int requestId, const QImage& img){
	Q_UNUSED(requestId);
	//QImage scaledImage = img.scaled(ui->viewfinder->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

	//ui->lastImagePreviewLabel->setPixmap(QPixmap::fromImage(scaledImage));

	// Display captured image for 4 seconds.
	displayCapturedImage();
	QTimer::singleShot(4000, this, SLOT(displayViewfinder()));
}



void Camera::record(){
	mediaRecorder->record();
	updateRecordTime();
}

void Camera::pause(){
	mediaRecorder->pause();
}

void Camera::stop(){
	mediaRecorder->stop();
}

void Camera::setMuted(bool muted){
	mediaRecorder->setMuted(muted);
}

void Camera::toggleLock(){
	switch (camera->lockStatus()) {
		case QCamera::Searching:
		case QCamera::Locked:
			camera->unlock();
			break;
		case QCamera::Unlocked:
			camera->searchAndLock();
	}
}

void Camera::updateLockStatus(QCamera::LockStatus status, QCamera::LockChangeReason reason){
	QColor indicationColor = Qt::black;
	switch (status) {
		case QCamera::Searching:
			indicationColor = Qt::yellow;
			//ui->statusbar->showMessage(tr("Focusing...")); ui->lockButton->setText(tr("Focusing..."));
			break;
		case QCamera::Locked:
			indicationColor = Qt::darkGreen;
			//ui->lockButton->setText(tr("Unlock")); ui->statusbar->showMessage(tr("Focused"), 2000);
			break;
		case QCamera::Unlocked:
			indicationColor = reason == QCamera::LockFailed ? Qt::red : Qt::black;
			//ui->lockButton->setText(tr("Focus"));
			if (reason == QCamera::LockFailed)
				//ui->statusbar->showMessage(tr("Focus Failed"), 2000);
				;
	}
	//QPalette palette = ui->lockButton->palette();
	//palette.setColor(QPalette::ButtonText, indicationColor);
	//ui->lockButton->setPalette(palette);
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
	//QMessageBox::warning(this, tr("Image Capture Error"), errorString);
	isCapturingImage = false;
}

void Camera::startCamera()
{
	camera->start();
}

void Camera::stopCamera()
{
	camera->stop();
}

void Camera::updateCaptureMode()
{
//	int tabIndex = ui->captureWidget->currentIndex();
	QCamera::CaptureModes captureMode =  QCamera::CaptureStillImage ; //QCamera::CaptureVideo;

	if (camera->isCaptureModeSupported(captureMode)){
		camera->setCaptureMode(captureMode);
	}
}

void Camera::updateCameraState(QCamera::State state)
{
	switch (state) {
		case QCamera::ActiveState:
//			ui->actionStartCamera->setEnabled(false);
	//		ui->actionStopCamera->setEnabled(true);
		//	ui->captureWidget->setEnabled(true);
//			ui->actionSettings->setEnabled(true);
			break;
		case QCamera::UnloadedState:
			break;
		case QCamera::LoadedState:
//			ui->actionStartCamera->setEnabled(true);
	//		ui->actionStopCamera->setEnabled(false);
		//	ui->captureWidget->setEnabled(false);
//			ui->actionSettings->setEnabled(false);
			break;
	}
}

void Camera::updateRecorderState(QMediaRecorder::State state)
{
	switch (state) {
		case QMediaRecorder::StoppedState:
//			ui->recordButton->setEnabled(true);
	//		ui->pauseButton->setEnabled(true);
		//	ui->stopButton->setEnabled(false);
			break;
		case QMediaRecorder::PausedState:
//			ui->recordButton->setEnabled(true);
	//		ui->pauseButton->setEnabled(false);
		//	ui->stopButton->setEnabled(true);
			break;
		case QMediaRecorder::RecordingState:
//			ui->recordButton->setEnabled(false);
	//		ui->pauseButton->setEnabled(true);
		//	ui->stopButton->setEnabled(true);
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
}

void Camera::displayCameraError()
{
	//QMessageBox::warning(this, tr("Camera error"), camera->errorString());
}


void Camera::displayViewfinder()
{
	//ui->stackedWidget->setCurrentIndex(0);
}

void Camera::displayCapturedImage()
{
	//ui->stackedWidget->setCurrentIndex(1);
}

void Camera::readyForCapture(bool ready)
{
	(void)ready;
	//ui->takeImageButton->setEnabled(ready);
}

void Camera::imageSaved(int id, const QString &fileName)
{
	Q_UNUSED(id);
	Q_UNUSED(fileName);

	isCapturingImage = false;
	if (applicationExiting){
		//close();
	}
}
