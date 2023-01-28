#include "Camera.hpp"
#include "ui_Camera.h"
//#include "camera/CameraSettings.hpp"

//#include "camera/PoorMansProbe.hpp"
#include "zbar/ZBarScanner.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "uptime/ConnectionType.hpp"

//#include <QMediaService>
#include <QMediaRecorder>
#include <QCamera>
//#include <QCameraViewfinder>
#include <QMediaMetaData>
#include <QMessageBox>
#include <QPalette>

#include <QtWidgets>

//Q_DECLARE_METATYPE(QCameraInfo)


Camera::Camera(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::Camera)
	, camera(nullptr)
//	, imageCapture(nullptr)
	, mediaRecorder(nullptr)
//	, videoProbe(nullptr)
//	, poorVideoProbe(nullptr)
	, zbar(nullptr)
	, isCapturingImage(false)
	, applicationExiting(false)
	, videoDevicesGroup(nullptr)
{
	OC_METHODGATE();
	ui->setupUi(this);
	ui->stackedWidgetScreens->setCurrentWidget(ui->pageCameraList);
	if(!connect(&cl, SIGNAL(cameraDevicesChanged()), this, SLOT(onCameraDevicesChanged()), OC_CONTYPE)) {
		qWarning()<<"ERROR: could not connect";
	}
}


Camera::~Camera()
{
	OC_METHODGATE();
//	delete videoProbe;
//	videoProbe=nullptr;
//	delete poorVideoProbe;
//	poorVideoProbe=nullptr;
	delete zbar;
	zbar=nullptr;
	delete videoDevicesGroup;
	videoDevicesGroup=nullptr;
	delete mediaRecorder;
	mediaRecorder=nullptr;
//	delete imageCapture;
//	imageCapture=nullptr;
	delete camera;
	camera=nullptr;
}
/*

void Camera::setCamera(const QCameraInfo &ci)
{
	OC_METHODGATE();
	//Free old camera
	{
		delete camera;
		camera=nullptr;
		delete mediaRecorder;
		mediaRecorder=nullptr;
//		delete imageCapture;
//		imageCapture=nullptr;
	}
	if(ci.isNull()) {
		ui->stackedWidgetScreens->setCurrentWidget(ui->pageCameraList);
		qDebug()<<"Selected camera is invalid, locking down UI";
	} else {
		{
			camera = OC_NEW QCamera(ci);
			if(nullptr!=camera) {
				cameraInfo=ci;
				qDebug()<<"Using camera: "<<cameraInfo;
				if(!connect(camera, SIGNAL(stateChanged(QCamera::State)), this, SLOT(updateCameraState(QCamera::State)), OC_CONTYPE)) {
					qWarning()<<"ERROR: could not connect";
				}
				if(!connect(camera, SIGNAL(error(QCamera::Error)), this, SLOT(displayCameraError()), OC_CONTYPE)) {
					qWarning()<<"ERROR: could not connect";
				}
				if(!connect(ui->exposureCompensation, SIGNAL(valueChanged(int)), SLOT(setExposureCompensation(int)), OC_CONTYPE)) {
					qWarning()<<"ERROR: could not connect";
				}
				if(!connect(camera, SIGNAL(lockStatusChanged(QCamera::LockStatus,QCamera::LockChangeReason)), this, SLOT(updateLockStatus(QCamera::LockStatus,QCamera::LockChangeReason)), OC_CONTYPE)) {
					qWarning()<<"ERROR: could not connect";
				}
				updateCameraState(camera->state());
				updateLockStatus(camera->lockStatus(), QCamera::UserRequest);
				ui->stackedWidgetScreens->setCurrentWidget(ui->pageViewfinder);

				if(camera->isCaptureModeSupported(QCamera::CaptureVideo)) {
					mediaRecorder = OC_NEW QMediaRecorder(camera);
					if(nullptr!=mediaRecorder) {
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

						if(!connect(mediaRecorder, SIGNAL(durationChanged(qint64)), this, SLOT(updateRecordTime()), OC_CONTYPE)) {
							qWarning()<<"ERROR: could not connect";
						}
						if(!connect(mediaRecorder, SIGNAL(error(QMediaRecorder::Error)), this, SLOT(displayRecorderError()), OC_CONTYPE)) {
							qWarning()<<"ERROR: could not connect";
						}

						if(!connect(mediaRecorder, SIGNAL(stateChanged(QMediaRecorder::State)), this, SLOT(updateRecorderState(QMediaRecorder::State)), OC_CONTYPE)) {
							qWarning()<<"ERROR: could not connect";
						}
						mediaRecorder->setMetaData(QMediaMetaData::Title, QVariant(QLatin1String("OctoMY™")));
						updateRecorderState(mediaRecorder->state());
					}
				}
				if(camera->isCaptureModeSupported(QCamera::CaptureStillImage)) {
					imageCapture = OC_NEW QCameraImageCapture(camera);

					if(nullptr!=imageCapture) {
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
						if(!connect(imageCapture, SIGNAL(readyForCaptureChanged(bool)), this, SLOT(readyForCapture(bool)), OC_CONTYPE)) {
							qWarning()<<"ERROR: could not connect";
						}
						if(!connect(imageCapture, SIGNAL(imageCaptured(int,QImage)), this, SLOT(processCapturedImage(int,QImage)), OC_CONTYPE)) {
							qWarning()<<"ERROR: could not connect";
						}
						if(!connect(imageCapture, SIGNAL(imageSaved(int,QString)), this, SLOT(imageSaved(int,QString)), OC_CONTYPE)) {
							qWarning()<<"ERROR: could not connect";
						}
						if(!connect(imageCapture, SIGNAL(error(int,QCameraImageCapture::Error,QString)), this, SLOT(displayCaptureError(int,QCameraImageCapture::Error,QString)), OC_CONTYPE)) {
							qWarning()<<"ERROR: could not connect";
						}
					}
				}


				//TODO: Handles iOS
#ifdef Q_OS_ANDROID
				//#if 1
				camera->setViewfinder(ui->viewfinder);
				if(nullptr != videoProbe) {
					videoProbe->deleteLater();
					videoProbe=0;
				}
				videoProbe = OC_NEW QVideoProbe(this);
				if (videoProbe->setSource(camera)) {
					// Probing succeeded, videoProbe->isValid() should be true.
					if(!connect(videoProbe, SIGNAL(videoFrameProbed(QVideoFrame)), this, SLOT(detectBarcodes(QVideoFrame)))) {
						qWarning()<<"ERROR: could not connect";
					}
				} else {
					qWarning()<<"ERROR: Could not set up probing of camera";
				}
#else
				if(nullptr!=poorVideoProbe) {
					poorVideoProbe->deleteLater();
					poorVideoProbe=nullptr;
				}
				poorVideoProbe=OC_NEW PoorMansProbe(this);
				if (poorVideoProbe->setSource(camera)) {
					// Probing succeeded, videoProbe->isValid() should be true.
					if(!connect(poorVideoProbe, SIGNAL(videoFrameProbed(QVideoFrame)), this, SLOT(detectBarcodes(QVideoFrame)), OC_CONTYPE)) {
						qWarning()<<"ERROR: could not connect";
					}
				} else {
					qWarning()<<"ERROR: Could not set up probing of camera";
				}
#endif
				updateCaptureMode();
				camera->start();
			} else {
				qWarning()<<"ERROR: Could not use camera: "<<ci;
			}
		}
	}
}
*/


void Camera::keyPressEvent(QKeyEvent * event)
{
	OC_METHODGATE();
	if (event->isAutoRepeat()) {
		return;
	}

	switch (event->key()) {
	case Qt::Key_CameraFocus:
		displayViewfinder();
		//camera->searchAndLock();
		event->accept();
		break;
	case Qt::Key_Camera:
		/*
		if (camera->captureMode() == QCamera::CaptureStillImage) {
			captureImage();
		} else {
			if (mediaRecorder->state() == QMediaRecorder::RecordingState) {
				stop();
			} else {
				record();
			}
		}
		*/
		event->accept();
		break;
	default:
		QWidget::keyPressEvent(event);
	}
}

void Camera::keyReleaseEvent(QKeyEvent *event)
{
	OC_METHODGATE();
	if (event->isAutoRepeat()) {
		return;
	}

	switch (event->key()) {
	case Qt::Key_CameraFocus:
		//camera->unlock();
		break;
	default:
		QWidget::keyReleaseEvent(event);
	}
}

void Camera::updateRecordTime()
{
	OC_METHODGATE();
	QString str = QString("Recorded %1 sec").arg(mediaRecorder->duration()/1000);
	showMessage(str);
}

void Camera::processCapturedImage(int requestId, const QImage& img)
{
	OC_METHODGATE();
	Q_UNUSED(requestId);
	QImage scaledImage = img.scaled(ui->viewfinder->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

	ui->lastImagePreviewLabel->setPixmap(QPixmap::fromImage(scaledImage));

	// Display captured image for 4 seconds.
	displayCapturedImage();
	QTimer::singleShot(4000, this, SLOT(displayViewfinder()));
}



void Camera::configureCaptureSettings()
{
	OC_METHODGATE();
	/*
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
	*/
}


void Camera::record()
{
	OC_METHODGATE();
	mediaRecorder->record();
	updateRecordTime();
}

void Camera::pause()
{
	OC_METHODGATE();
	mediaRecorder->pause();
}

void Camera::stop()
{
	OC_METHODGATE();
	mediaRecorder->stop();
}

void Camera::setMuted(bool muted)
{
	OC_METHODGATE();
	//mediaRecorder->setMuted(muted);
}

void Camera::tryFocus()
{
	OC_METHODGATE();
	/*
	switch (camera->lockStatus()) {
		case QCamera::Searching:
		case QCamera::Locked:
			camera->unlock();
			break;
		case QCamera::Unlocked:


	}
	*/
	//camera->searchAndLock();

}

/*
void Camera::updateLockStatus(QCamera::LockStatus status, QCamera::LockChangeReason reason)
{
	OC_METHODGATE();
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
		if (reason == QCamera::LockFailed) {
			showMessage(tr("Focus Failed"));
		}
	}

	QPalette palette = ui->toolButtonFocus->palette();
	palette.setColor(QPalette::ButtonText, indicationColor);
	ui->toolButtonFocus->setPalette(palette);
}
*/

void Camera::captureImage()
{
	OC_METHODGATE();
	isCapturingImage = true;
	//imageCapture->capture();
}


/*
void Camera::displayCaptureError(int id, const QCameraImageCapture::Error error, const QString &errorString)
{
	OC_METHODGATE();
	Q_UNUSED(id);
	Q_UNUSED(error);
	QMessageBox::warning(this, tr("Image Capture Error"), errorString);
	isCapturingImage = false;
}
*/

void Camera::startCamera()
{
	OC_METHODGATE();
	camera->start();
	ui->viewfinder->show();
}

void Camera::stopCamera()
{
	OC_METHODGATE();
	ui->viewfinder->hide();
	camera->stop();
}


void Camera::updateCaptureMode()
{
	OC_METHODGATE();
	int tabIndex = 1; //ui->tabWidgetCapture->currentIndex();
	/*
	QCamera::CaptureModes captureMode = tabIndex == 0 ? QCamera::CaptureStillImage : QCamera::CaptureVideo;

	if (camera->isCaptureModeSupported(captureMode)) {
		camera->setCaptureMode(captureMode);
	}
	*/
}


/*
void Camera::updateCameraState(QCamera::State state)
{
	OC_METHODGATE();
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
*/

/*
void Camera::updateRecorderState(QMediaRecorder::State state)
{
	OC_METHODGATE();
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
*/

void Camera::setExposureCompensation(int index)
{
	OC_METHODGATE();
	//camera->exposure()->setExposureCompensation(index*0.5);
}

void Camera::displayRecorderError()
{
	OC_METHODGATE();
	//QMessageBox::warning(this, tr("Capture error"), mediaRecorder->errorString());
	qWarning()<<"WARNING: Capture error :"<<mediaRecorder->errorString();
}

void Camera::displayCameraError()
{
	OC_METHODGATE();
	//QMessageBox::warning(this, tr("Camera error"), camera->errorString());
	qWarning()<<"WARNING: Camera error :"<<camera->errorString();
}

void Camera::updateCameraDevice(QListWidgetItem *item)
{
	OC_METHODGATE();
	//setCamera(qvariant_cast<QCameraInfo>(item->data(Qt::UserRole)));
}

void Camera::displayViewfinder()
{
	OC_METHODGATE();
	ui->stackedWidgetScreens->setCurrentWidget(ui->pageViewfinder);
}

void Camera::displayCapturedImage()
{
	OC_METHODGATE();
	ui->stackedWidgetScreens->setCurrentWidget(ui->pageImagePreview);
}

void Camera::readyForCapture(bool ready)
{
	OC_METHODGATE();
	ui->toolButtonCaptureImage->setEnabled(ready);
}

void Camera::imageSaved(int id, const QString &fileName)
{
	OC_METHODGATE();
	Q_UNUSED(id);
	Q_UNUSED(fileName);

	isCapturingImage = false;
	if (applicationExiting) {
		close();
	}
}

void Camera::closeEvent(QCloseEvent *event)
{
	OC_METHODGATE();
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
	OC_METHODGATE();
	ui->labelMessage->setText(msg);
}



void Camera::detectBarcodes(const QVideoFrame &frame)
{
	OC_METHODGATE();
	//qDebug()<<"GOT FRAME "<<frame;
	if(nullptr==zbar) {
		zbar=OC_NEW ZBarScanner;
	}
	if(nullptr!=zbar) {
		zbar->scan(frame);
	}
}


void Camera::onCameraDevicesChanged()
{
	OC_METHODGATE();
	/*
	QList<QCameraInfo> cams=QCameraInfo::availableCameras();
	//const int l=cams.size(); qDebug()<<"CAMS FOUND ON POLL: "<<l;
	const QCameraInfo &def=QCameraInfo::defaultCamera();
	const QCameraInfo last=nullptr!=camera?cameraInfo:def;
	QIcon ic(":/icons/eye.svg") ;
	ui->listWidgetCameras->clear();
	foreach (const QCameraInfo &ci, cams) {
		QListWidgetItem *item = OC_NEW QListWidgetItem(ui->listWidgetCameras);
		item->setText( ci.description() );
		item->setToolTip( ci.deviceName());
		item->setIcon(ic);
		item->setData(Qt::UserRole, QVariant::fromValue(ci));
		if(!connect(ui->listWidgetCameras, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(updateCameraDevice(QListWidgetItem*)), OC_CONTYPE)) {
			qWarning()<<"ERROR: could not connect";
		}
		if (ci == last) {
			item->setSelected(true);
		}
	}
	
	//if(nullptr != camera){
	//setCamera(last);
//	}
	const int num=ui->listWidgetCameras->count();
	const bool camsAvailable=num>0;
	//qDebug()<<"CAMS AVAILABLE: num="<<num<<": "<<camsAvailable<<": "<<camListHashNew<<" vs. "<<camListHash;
	ui->listWidgetCameras->setVisible(camsAvailable);
	ui->labelNoCamerasAvailable->setVisible(!camsAvailable);
	*/
}





//////


/*
QAudioEncoderSettings Camera::getAudioSettings() const
{
	OC_METHODGATE();
	QAudioEncoderSettings settings = mediaRecorder->audioSettings();
	settings.setCodec(boxValue(ui->audioCodecBox).toString());
	settings.setQuality(QMultimedia::EncodingQuality(ui->audioQualitySlider->value()));
	settings.setSampleRate(boxValue(ui->audioSampleRateBox).toInt());
	return settings;
}
*/


/*
void Camera::setAudioSettings(const QAudioEncoderSettings &audioSettings)
{
	OC_METHODGATE();
	selectComboBoxItem(ui->audioCodecBox, QVariant(audioSettings.codec()));
	selectComboBoxItem(ui->audioSampleRateBox, QVariant(audioSettings.sampleRate()));
	ui->audioQualitySlider->setValue(audioSettings.quality());
}
*/


/*
QVideoEncoderSettings Camera::getVideoSettings() const
{
	OC_METHODGATE();
	QVideoEncoderSettings settings = mediaRecorder->videoSettings();
	settings.setCodec(boxValue(ui->videoCodecBox).toString());
	settings.setQuality(QMultimedia::EncodingQuality(ui->videoQualitySlider->value()));
	//settings.setResolution(boxValue(ui->videoResolutionBox).toSize());
	settings.setFrameRate(boxValue(ui->videoFramerateBox).value<qreal>());

	return settings;
}
*/


/*
void Camera::setVideoSettings(const QVideoEncoderSettings &videoSettings)
{
	OC_METHODGATE();
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
*/


QString Camera::format() const
{
	OC_METHODGATE();
	return boxValue(ui->containerFormatBox).toString();
}

void Camera::setFormat(const QString &format)
{
	selectComboBoxItem(ui->containerFormatBox, QVariant(format));
}



/*
QImageEncoderSettings Camera::getImageSettings() const
{
	OC_METHODGATE();
	QImageEncoderSettings settings = imageCapture->encodingSettings();
	settings.setCodec(boxValue(ui->imageCodecBox).toString());
	settings.setQuality(QMultimedia::EncodingQuality(ui->imageQualitySlider->value()));
	//settings.setResolution(boxValue(ui->imageResolutionBox).toSize());
	//imageSettings=settings;
	return settings;
}
*/

/*
void Camera::setImageSettings(const QImageEncoderSettings &imageSettings)
{
	OC_METHODGATE();
	selectComboBoxItem(ui->imageCodecBox, QVariant(imageSettings.codec()));
	selectComboBoxItem(ui->imageResolutionBox, QVariant(imageSettings.resolution()));
	ui->imageQualitySlider->setValue(imageSettings.quality());
}
*/


QVariant Camera::boxValue(const QComboBox *box) const
{
	OC_METHODGATE();
	int idx = box->currentIndex();
	if (idx == -1) {
		return QVariant();
	}

	return box->itemData(idx);
}

void Camera::selectComboBoxItem(QComboBox *box, const QVariant &value)
{
	OC_METHODGATE();
	for (int i = 0; i < box->count(); ++i) {
		if (box->itemData(i) == value) {
			box->setCurrentIndex(i);
			break;
		}
	}
}


void Camera::on_toolButtonCaptureImage_clicked()
{
	OC_METHODGATE();
	captureImage();
}

void Camera::on_toolButtonRecordVideo_toggled(bool checked)
{
	OC_METHODGATE();
	checked?record():stop();
}

void Camera::on_toolButtonBack_clicked()
{
	OC_METHODGATE();
	/*
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
	*/
	ui->stackedWidgetScreens->setCurrentWidget(ui->pageViewfinder);
	
}

void Camera::on_toolButtonSettings_clicked()
{
	OC_METHODGATE();
	/*
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
*/
	ui->stackedWidgetScreens->setCurrentWidget(ui->pageSettings);
}

void Camera::on_toolButtonFocus_clicked()
{
	OC_METHODGATE();
	tryFocus();
}
