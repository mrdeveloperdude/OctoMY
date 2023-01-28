#include "CameraSettings.hpp"
#include "ui_CameraSettings.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QComboBox>
#include <QDebug>

//#include <QCameraImageCapture>
//#include <QMediaRecorder>
//#include <QMediaService>


CameraSettings::CameraSettings(/*QMediaRecorder *mediaRecorder, QCameraImageCapture *imagecapture,*/ QWidget *parent)
	: QDialog(parent)
	, ui(OC_NEW Ui::VideoSettingsUi)
//	, mediaRecorder(mediaRecorder)
//	, imagecapture(imagecapture)
{
	OC_METHODGATE();
	ui->setupUi(this);

	//audio codecs
	ui->audioCodecBox->addItem(tr("Default audio codec"), QVariant(QString()));
	/*
	foreach (const QString &codecName, mediaRecorder->supportedAudioCodecs()) {
		QString description = mediaRecorder->audioCodecDescription(codecName);
		ui->audioCodecBox->addItem(codecName+": "+description, QVariant(codecName));
	}*/

	//sample rate:
	/*
	foreach (int sampleRate, mediaRecorder->supportedAudioSampleRates()) {
		ui->audioSampleRateBox->addItem(QString::number(sampleRate), QVariant(sampleRate));
	}*/

	/*
	ui->audioQualitySlider->setRange(0, int(QMultimedia::VeryHighQuality));
	*/

	//video codecs
	ui->videoCodecBox->addItem(tr("Default video codec"), QVariant(QString()));
	/*
	foreach (const QString &codecName, mediaRecorder->supportedVideoCodecs()) {
		QString description = mediaRecorder->videoCodecDescription(codecName);
		ui->videoCodecBox->addItem(codecName+": "+description, QVariant(codecName));
	}*/

	/*
	ui->videoQualitySlider->setRange(0, int(QMultimedia::VeryHighQuality));
*/
	{
		ui->videoResolutionBox->addItem(tr("Default"));
		/*
		QList<QSize> supportedResolutions = mediaRecorder->supportedResolutions();
		foreach (const QSize &resolution, supportedResolutions) {
			ui->videoResolutionBox->addItem(QString("%1x%2").arg(resolution.width()).arg(resolution.height()),
											QVariant(resolution));
		}
		*/
	}
	
		
	
	ui->videoFramerateBox->addItem(tr("Default"));
	/*
	QList<qreal> supportedFrameRates = mediaRecorder->supportedFrameRates();
	qreal rate;
	foreach (rate, supportedFrameRates) {
		QString rateString = QString("%1").arg(rate, 0, 'f', 2);
		ui->videoFramerateBox->addItem(rateString, QVariant(rate));
	}
	*/

	//containers
	ui->containerFormatBox->addItem(tr("Default container"), QVariant(QString()));
	/*
	foreach (const QString &format, mediaRecorder->supportedContainers()) {
		ui->containerFormatBox->addItem(format+":"+mediaRecorder->containerDescription(format), QVariant(format));
	}
	*/



	//image codecs
	ui->imageCodecBox->addItem(tr("Default image format"), QVariant(QString()));
	/*
	foreach(const QString &codecName, imagecapture->supportedImageCodecs()) {
		QString description = imagecapture->imageCodecDescription(codecName);
		ui->imageCodecBox->addItem(codecName+": "+description, QVariant(codecName));
	}
	*/

	/*
	ui->imageQualitySlider->setRange(0, int(QMultimedia::VeryHighQuality));
	{
		ui->imageResolutionBox->addItem(tr("Default Resolution"));
		QList<QSize> supportedResolutions = imagecapture->supportedResolutions();
		foreach(const QSize &resolution, supportedResolutions) {
			ui->imageResolutionBox->addItem(QString("%1x%2").arg(resolution.width()).arg(resolution.height()),
											QVariant(resolution));
		}
	}
	*/
}

CameraSettings::~CameraSettings()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}

void CameraSettings::changeEvent(QEvent *e)
{
	OC_METHODGATE();
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

/*
QAudioEncoderSettings CameraSettings::audioSettings() const
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
void CameraSettings::setAudioSettings(const QAudioEncoderSettings &audioSettings)
{
	OC_METHODGATE();
	selectComboBoxItem(ui->audioCodecBox, QVariant(audioSettings.codec()));
	selectComboBoxItem(ui->audioSampleRateBox, QVariant(audioSettings.sampleRate()));
	ui->audioQualitySlider->setValue(audioSettings.quality());
}
*/


/*
QVideoEncoderSettings CameraSettings::videoSettings() const
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
void CameraSettings::setVideoSettings(const QVideoEncoderSettings &videoSettings)
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


/*
QImageEncoderSettings CameraSettings::imageSettings() const
{
	OC_METHODGATE();
	QImageEncoderSettings settings = imagecapture->encodingSettings();
	settings.setCodec(boxValue(ui->imageCodecBox).toString());
	settings.setQuality(QMultimedia::EncodingQuality(ui->imageQualitySlider->value()));
	//settings.setResolution(boxValue(ui->imageResolutionBox).toSize());

	return settings;
}
*/

/*
void CameraSettings::setImageSettings(const QImageEncoderSettings &imageSettings)
{
	OC_METHODGATE();
	selectComboBoxItem(ui->imageCodecBox, QVariant(imageSettings.codec()));
	selectComboBoxItem(ui->imageResolutionBox, QVariant(imageSettings.resolution()));
	ui->imageQualitySlider->setValue(imageSettings.quality());
}
*/

QString CameraSettings::format() const
{
	OC_METHODGATE();
	return boxValue(ui->containerFormatBox).toString();
}

void CameraSettings::setFormat(const QString &format)
{
	OC_METHODGATE();
	selectComboBoxItem(ui->containerFormatBox, QVariant(format));
}



QVariant CameraSettings::boxValue(const QComboBox *box) const
{
	OC_METHODGATE();
	int idx = box->currentIndex();
	if (idx == -1) {
		return QVariant();
	}

	return box->itemData(idx);
}

void CameraSettings::selectComboBoxItem(QComboBox *box, const QVariant &value)
{
	OC_METHODGATE();
	for (int i = 0; i < box->count(); ++i) {
		if (box->itemData(i) == value) {
			box->setCurrentIndex(i);
			break;
		}
	}
}
