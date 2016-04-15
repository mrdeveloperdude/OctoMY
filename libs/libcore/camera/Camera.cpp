#include "Camera.hpp"
#include "ui_Camera.h"
#include "CameraSettings.hpp"

#include "PoorMansProbe.hpp"
#include "../libzbar/ZBarScanner.hpp"

#include "utility/Utility.hpp"

#include <QMediaService>
#include <QMediaRecorder>
#include <QCamera>
#include <QCameraViewfinder>
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
	, poorVideoProbe(0)
	, zbar(0)
	, isCapturingImage(false)
	, applicationExiting(false)
	, videoDevicesGroup(0)
{
	ui->setupUi(this);
	ui->stackedWidgetScreens->setCurrentWidget(ui->pageCameraList);
	if(!connect(&cl,SIGNAL(cameraDevicesChanged()),this,SLOT(onCameraDevicesChanged()))){
		qWarning()<<"ERROR: could not connect";
	}
}


Camera::~Camera()
{
	delete videoProbe;
	videoProbe=0;
	delete poorVideoProbe;
	poorVideoProbe=0;
	delete zbar;
	zbar=0;
	delete videoDevicesGroup;
	videoDevicesGroup=0;
	delete mediaRecorder;
	mediaRecorder=0;
	delete imageCapture;
	imageCapture=0;
	delete camera;
	camera=0;
}


void Camera::setCamera(const QCameraInfo &ci)
{
	//Free old camera
	{
		delete camera;
		camera=0;
		delete mediaRecorder;
		mediaRecorder=0;
		delete imageCapture;
		imageCapture=0;
	}
	if(ci.isNull()){
		ui->stackedWidgetScreens->setCurrentWidget(ui->pageCameraList);
		qDebug()<<"Selected camera is invalid, locking down UI";
	}
	else{
		{
			camera = new QCamera(ci);
			if(0!=camera){
				cameraInfo=ci;
				qDebug()<<"Using camera: "<<cameraInfo;
				if(!connect(camera, SIGNAL(stateChanged(QCamera::State)), this, SLOT(updateCameraState(QCamera::State)))){
					qWarning()<<"ERROR: could not connect";
				}
				if(!connect(camera, SIGNAL(error(QCamera::Error)), this, SLOT(displayCameraError()))){
					qWarning()<<"ERROR: could not connect";
				}
				if(!connect(ui->exposureCompensation, SIGNAL(valueChanged(int)), SLOT(setExposureCompensation(int)))){
					qWarning()<<"ERROR: could not connect";
				}
				if(!connect(camera, SIGNAL(lockStatusChanged(QCamera::LockStatus,QCamera::LockChangeReason)), this, SLOT(updateLockStatus(QCamera::LockStatus,QCamera::LockChangeReason)))){
					qWarning()<<"ERROR: could not connect";
				}
				updateCameraState(camera->state());
				updateLockStatus(camera->lockStatus(), QCamera::UserRequest);
				ui->stackedWidgetScreens->setCurrentWidget(ui->pageViewfinder);

				if(camera->isCaptureModeSupported(QCamera::CaptureVideo)){
					mediaRecorder = new QMediaRecorder(camera);
					if(0!=mediaRecorder){
						//mediaRecorder->setOutputLocation(QUrl::fromLocalFile("/tmp/video.mp4"));
						//audio codecs
						ui->audioCodecBox->addItem(tr("Default audio codec"), QVariant(QString()));
						foreach (const QString &codecName, mediaRecorder->supportedAudioCodecs()) {
							QString description = mediaRecorder->audioCodecDescription(codecName);
							ui->audioCodecBox->addItem(codecName+": "+description, QVariant(codecName));
						}

						//sample rate:
						foreach (int sampleRate, mediaRecorder->supportedAudioSampleRates()) {
							ui->audioSampleRateBox->addItem(QString::number(sampleRate), QVariant(sampleRate));
						}
						ui->audioQualitySlider->setRange(0, int(QMultimedia::VeryHighQuality));

						//video codecs
						ui->videoCodecBox->addItem(tr("Default video codec"), QVariant(QString()));
						foreach (const QString &codecName, mediaRecorder->supportedVideoCodecs()) {
							QString description = mediaRecorder->videoCodecDescription(codecName);
							ui->videoCodecBox->addItem(codecName+": "+description, QVariant(codecName));
						}
						ui->videoQualitySlider->setRange(0, int(QMultimedia::VeryHighQuality));
						{
							ui->videoResolutionBox->addItem(tr("Default"));
							QList<QSize> supportedResolutions = mediaRecorder->supportedResolutions();
							foreach (const QSize &resolution, supportedResolutions) {
								ui->videoResolutionBox->addItem(QString("%1x%2").arg(resolution.width()).arg(resolution.height()), QVariant(resolution));
							}
						}
						ui->videoFramerateBox->addItem(tr("Default"));
						QList<qreal> supportedFrameRates = mediaRecorder->supportedFrameRates();
						qreal rate;
						foreach (rate, supportedFrameRates) {
							QString rateString = QString("%1").arg(rate, 0, 'f', 2);
							ui->videoFramerateBox->addItem(rateString, QVariant(rate));
						}

						//containers
						ui->containerFormatBox->addItem(tr("Default container"), QVariant(QString()));
						foreach (const QString &format, mediaRecorder->supportedContainers()) {
							ui->containerFormatBox->addItem(format+":"+mediaRecorder->containerDescription(format), QVariant(format));
						}

						if(!connect(mediaRecorder, SIGNAL(durationChanged(qint64)), this, SLOT(updateRecordTime()))){
							qWarning()<<"ERROR: could not connect";
						}
						if(!connect(mediaRecorder, SIGNAL(error(QMediaRecorder::Error)), this, SLOT(displayRecorderError()))){
							qWarning()<<"ERROR: could not connect";
						}

						if(!connect(mediaRecorder, SIGNAL(stateChanged(QMediaRecorder::State)), this, SLOT(updateRecorderState(QMediaRecorder::State)))){
							qWarning()<<"ERROR: could not connect";
						}
						mediaRecorder->setMetaData(QMediaMetaData::Title, QVariant(QLatin1String("OctoMY™")));
						updateRecorderState(mediaRecorder->state());
					}
				}
				if(camera->isCaptureModeSupported(QCamera::CaptureStillImage)){
					imageCapture = new QCameraImageCapture(camera);

					if(0!=imageCapture){
						//image codecs
						ui->imageCodecBox->addItem(tr("Default image format"), QVariant(QString()));
						foreach(const QString &codecName, imageCapture->supportedImageCodecs()) {
							QString description = imageCapture->imageCodecDescription(codecName);
							ui->imageCodecBox->addItem(codecName+": "+description, QVariant(codecName));
						}

						ui->imageQualitySlider->setRange(0, int(QMultimedia::VeryHighQuality));
						{
							ui->imageResolutionBox->addItem(tr("Default Resolution"));
							QList<QSize> supportedResolutions = imageCapture->supportedResolutions();
							foreach(const QSize &resolution, supportedResolutions) {
								ui->imageResolutionBox->addItem(QString("%1x%2").arg(resolution.width()).arg(resolution.height()), QVariant(resolution));
							}
						}
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
					}
				}


				//TODO: Handles iOS
#ifdef Q_OS_ANDROID
				//#if 1
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
#else
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
#endif
				updateCaptureMode();
				camera->start();
			}
			else{
				qWarning()<<"ERROR: Could not use camera: "<<ci;
			}
		}
	}
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
				captureImage();
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
	QImage scaledImage = img.scaled(ui->viewfinder->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

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

		mediaRecorder->setEncodingSettings( audioSettings, videoSettings, videoContainerFormat);

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

void Camera::tryFocus()
{
	/*
	switch (camera->lockStatus()) {
		case QCamera::Searching:
		case QCamera::Locked:
			camera->unlock();
			break;
		case QCamera::Unlocked:


	}
	*/
	camera->searchAndLock();

}

void Camera::updateLockStatus(QCamera::LockStatus status, QCamera::LockChangeReason reason)
{
	QColor indicationColor = Qt::black;

	switch (status) {
		case QCamera::Searching:
			indicationColor = Qt::yellow;
			showMessage(tr("Focusing..."));
			break;
		case QCamera::Locked:
			indicationColor = Qt::darkGreen;
			showMessage(tr("Focused"));
			break;
		case QCamera::Unlocked:
			indicationColor = reason == QCamera::LockFailed ? Qt::red : Qt::black;
			if (reason == QCamera::LockFailed){
				showMessage(tr("Focus Failed"));
			}
	}

	QPalette palette = ui->toolButtonFocus->palette();
	palette.setColor(QPalette::ButtonText, indicationColor);
	ui->toolButtonFocus->setPalette(palette);
}

void Camera::captureImage()
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
	camera->start();
	ui->viewfinder->show();
}

void Camera::stopCamera()
{
	ui->viewfinder->hide();
	camera->stop();
}

void Camera::updateCaptureMode()
{
	int tabIndex = 1; //ui->tabWidgetCapture->currentIndex();
	QCamera::CaptureModes captureMode = tabIndex == 0 ? QCamera::CaptureStillImage : QCamera::CaptureVideo;

	if (camera->isCaptureModeSupported(captureMode)){
		camera->setCaptureMode(captureMode);
	}
}



void Camera::updateCameraState(QCamera::State state)
{
	switch (state) {
		case QCamera::ActiveState:
			//ui->tabWidgetCapture->setEnabled(true);
			//			ui->pushButtonSettings->setEnabled(true);
			break;
		case QCamera::UnloadedState:
		case QCamera::LoadedState:
			//ui->tabWidgetCapture->setEnabled(false);
			//		ui->pushButtonSettings->setEnabled(false);
			break;
	};
}

void Camera::updateRecorderState(QMediaRecorder::State state)
{
	QSignalBlocker sb1(ui->toolButtonRecordVideo);
	QSignalBlocker sb2(ui->toolButtonCaptureImage);
	QSignalBlocker sb3(ui->toolButtonPauseVideoRecording);
	switch (state) {
		case QMediaRecorder::StoppedState:
			ui->toolButtonRecordVideo->setEnabled(true);
			ui->toolButtonRecordVideo->setChecked(false);
			ui->toolButtonCaptureImage->setEnabled(true);
			ui->toolButtonPauseVideoRecording->setEnabled(false);
			ui->toolButtonPauseVideoRecording->setChecked(false);
			break;
		case QMediaRecorder::PausedState:
			ui->toolButtonRecordVideo->setEnabled(true);
			ui->toolButtonRecordVideo->setChecked(true);
			ui->toolButtonCaptureImage->setEnabled(false);
			ui->toolButtonPauseVideoRecording->setEnabled(true);
			ui->toolButtonPauseVideoRecording->setChecked(true);
			break;
		case QMediaRecorder::RecordingState:
			ui->toolButtonRecordVideo->setEnabled(true);
			ui->toolButtonRecordVideo->setChecked(true);
			ui->toolButtonCaptureImage->setEnabled(false);
			ui->toolButtonPauseVideoRecording->setEnabled(true);
			ui->toolButtonPauseVideoRecording->setChecked(false);
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

void Camera::updateCameraDevice(QListWidgetItem *item)
{
	setCamera(qvariant_cast<QCameraInfo>(item->data(Qt::UserRole)));
}

void Camera::displayViewfinder()
{
	ui->stackedWidgetScreens->setCurrentWidget(ui->pageViewfinder);
}

void Camera::displayCapturedImage()
{
	ui->stackedWidgetScreens->setCurrentWidget(ui->pageImagePreview);
}

void Camera::readyForCapture(bool ready)
{
	ui->toolButtonCaptureImage->setEnabled(ready);
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


void Camera::showMessage(QString msg)
{
	ui->labelMessage->setText(msg);
}



void Camera::detectBarcodes(const QVideoFrame &frame)
{
	//qDebug()<<"GOT FRAME "<<frame;
	if(0==zbar){
		zbar=new ZBarScanner;
	}
	if(0!=zbar){
		zbar->scan(frame);
	}
}


void Camera::onCameraDevicesChanged()
{
	QList<QCameraInfo> cams=QCameraInfo::availableCameras();
	//const int l=cams.size(); qDebug()<<"CAMS FOUND ON POLL: "<<l;
	const QCameraInfo &def=QCameraInfo::defaultCamera();
	const QCameraInfo last=0!=camera?cameraInfo:def;
	QIcon ic(":/icons/eye.svg") ;
	ui->listWidgetCameras->clear();
	foreach (const QCameraInfo &ci, cams) {
		QListWidgetItem *item = new QListWidgetItem(ui->listWidgetCameras);
		item->setText( ci.description() );
		item->setToolTip( ci.deviceName());
		item->setIcon(ic);
		item->setData(Qt::UserRole, QVariant::fromValue(ci));
		if(!connect(ui->listWidgetCameras, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(updateCameraDevice(QListWidgetItem*)))){
			qWarning()<<"ERROR: could not connect";
		}
		if (ci == last){
			item->setSelected(true);
		}
	}
	/*
if(0!=camera){
	setCamera(last);
}*/
	const int num=ui->listWidgetCameras->count();
	const bool camsAvailable=num>0;
	//qDebug()<<"CAMS AVAILABLE: num="<<num<<": "<<camsAvailable<<": "<<camListHashNew<<" vs. "<<camListHash;
	ui->listWidgetCameras->setVisible(camsAvailable);
	ui->labelNoCamerasAvailable->setVisible(!camsAvailable);
}





//////





QAudioEncoderSettings Camera::getAudioSettings() const
{
	QAudioEncoderSettings settings = mediaRecorder->audioSettings();
	settings.setCodec(boxValue(ui->audioCodecBox).toString());
	settings.setQuality(QMultimedia::EncodingQuality(ui->audioQualitySlider->value()));
	settings.setSampleRate(boxValue(ui->audioSampleRateBox).toInt());
	return settings;
}

void Camera::setAudioSettings(const QAudioEncoderSettings &audioSettings)
{
	selectComboBoxItem(ui->audioCodecBox, QVariant(audioSettings.codec()));
	selectComboBoxItem(ui->audioSampleRateBox, QVariant(audioSettings.sampleRate()));
	ui->audioQualitySlider->setValue(audioSettings.quality());
}

QVideoEncoderSettings Camera::getVideoSettings() const
{
	QVideoEncoderSettings settings = mediaRecorder->videoSettings();
	settings.setCodec(boxValue(ui->videoCodecBox).toString());
	settings.setQuality(QMultimedia::EncodingQuality(ui->videoQualitySlider->value()));
	//settings.setResolution(boxValue(ui->videoResolutionBox).toSize());
	settings.setFrameRate(boxValue(ui->videoFramerateBox).value<qreal>());

	return settings;
}

void Camera::setVideoSettings(const QVideoEncoderSettings &videoSettings)
{
	selectComboBoxItem(ui->videoCodecBox, QVariant(videoSettings.codec()));
	selectComboBoxItem(ui->videoResolutionBox, QVariant(videoSettings.resolution()));
	ui->videoQualitySlider->setValue(videoSettings.quality());

	//special case for frame rate
	for (int i = 0; i < ui->videoFramerateBox->count(); ++i) {
		qreal itemRate = ui->videoFramerateBox->itemData(i).value<qreal>();
		if (qFuzzyCompare(itemRate, videoSettings.frameRate())) {
			ui->videoFramerateBox->setCurrentIndex(i);
			break;
		}
	}
}

QString Camera::format() const
{
	return boxValue(ui->containerFormatBox).toString();
}

void Camera::setFormat(const QString &format)
{
	selectComboBoxItem(ui->containerFormatBox, QVariant(format));
}




QImageEncoderSettings Camera::getImageSettings() const
{
	QImageEncoderSettings settings = imageCapture->encodingSettings();
	settings.setCodec(boxValue(ui->imageCodecBox).toString());
	settings.setQuality(QMultimedia::EncodingQuality(ui->imageQualitySlider->value()));
	//settings.setResolution(boxValue(ui->imageResolutionBox).toSize());
	//imageSettings=settings;
	return settings;
}

void Camera::setImageSettings(const QImageEncoderSettings &imageSettings)
{
	selectComboBoxItem(ui->imageCodecBox, QVariant(imageSettings.codec()));
	selectComboBoxItem(ui->imageResolutionBox, QVariant(imageSettings.resolution()));
	ui->imageQualitySlider->setValue(imageSettings.quality());
}

QVariant Camera::boxValue(const QComboBox *box) const
{
	int idx = box->currentIndex();
	if (idx == -1)
		return QVariant();

	return box->itemData(idx);
}

void Camera::selectComboBoxItem(QComboBox *box, const QVariant &value)
{
	for (int i = 0; i < box->count(); ++i) {
		if (box->itemData(i) == value) {
			box->setCurrentIndex(i);
			break;
		}
	}
}


void Camera::on_toolButtonCaptureImage_clicked(){
	captureImage();
}

void Camera::on_toolButtonRecordVideo_toggled(bool checked)
{
	checked?record():stop();
}

void Camera::on_toolButtonBack_clicked()
{
	{
		QImageEncoderSettings settings = imageCapture->encodingSettings();
		settings.setCodec(boxValue(ui->imageCodecBox).toString());
		settings.setQuality(QMultimedia::EncodingQuality(ui->imageQualitySlider->value()));
		settings.setResolution(boxValue(ui->imageResolutionBox).toSize());
		imageSettings=settings;
	}
	{
		QAudioEncoderSettings settings = mediaRecorder->audioSettings();
		settings.setCodec(boxValue(ui->audioCodecBox).toString());
		settings.setQuality(QMultimedia::EncodingQuality(ui->audioQualitySlider->value()));
		settings.setSampleRate(boxValue(ui->audioSampleRateBox).toInt());

		audioSettings=settings;
	}

	{
		QVideoEncoderSettings settings = mediaRecorder->videoSettings();
		settings.setCodec(boxValue(ui->videoCodecBox).toString());
		settings.setQuality(QMultimedia::EncodingQuality(ui->videoQualitySlider->value()));
		settings.setResolution(boxValue(ui->videoResolutionBox).toSize());
		settings.setFrameRate(boxValue(ui->videoFramerateBox).value<qreal>());

		videoSettings=settings;
	}
	ui->stackedWidgetScreens->setCurrentWidget(ui->pageViewfinder);
}

void Camera::on_toolButtonSettings_clicked()
{
	{
		selectComboBoxItem(ui->audioCodecBox, QVariant(audioSettings.codec()));
		selectComboBoxItem(ui->audioSampleRateBox, QVariant(audioSettings.sampleRate()));
		ui->audioQualitySlider->setValue(audioSettings.quality());
	}

	{
		selectComboBoxItem(ui->videoCodecBox, QVariant(videoSettings.codec()));
		selectComboBoxItem(ui->videoResolutionBox, QVariant(videoSettings.resolution()));
		ui->videoQualitySlider->setValue(videoSettings.quality());

		//special case for frame rate
		for (int i = 0; i < ui->videoFramerateBox->count(); ++i) {
			qreal itemRate = ui->videoFramerateBox->itemData(i).value<qreal>();
			if (qFuzzyCompare(itemRate, videoSettings.frameRate())) {
				ui->videoFramerateBox->setCurrentIndex(i);
				break;
			}
		}
	}
	{
		selectComboBoxItem(ui->imageCodecBox, QVariant(imageSettings.codec()));
		selectComboBoxItem(ui->imageResolutionBox, QVariant(imageSettings.resolution()));
		ui->imageQualitySlider->setValue(imageSettings.quality());
	}

	ui->stackedWidgetScreens->setCurrentWidget(ui->pageSettings);
}

void Camera::on_toolButtonFocus_clicked()
{
	tryFocus();
}
