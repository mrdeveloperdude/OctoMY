#ifndef CAMERASETTINGS_H
#define CAMERASETTINGS_H

#include <QDialog>

//#include <QAudioEncoderSettings>
//#include <QVideoEncoderSettings>

QT_BEGIN_NAMESPACE
class QComboBox;
class QMediaRecorder;
class QCameraImageCapture;
namespace Ui
{
class VideoSettingsUi;
}
QT_END_NAMESPACE

class CameraSettings : public QDialog
{
	Q_OBJECT
private:
	Ui::VideoSettingsUi *ui;
	/*
	QMediaRecorder *mediaRecorder;
	QCameraImageCapture *imagecapture;
	*/
	
public:
	CameraSettings(/*QMediaRecorder *mediaRecorder, QCameraImageCapture *imagecapture, */ QWidget *parent = nullptr);
	~CameraSettings();
/*
	QAudioEncoderSettings audioSettings() const;
	void setAudioSettings(const QAudioEncoderSettings&);

	QVideoEncoderSettings videoSettings() const;
	void setVideoSettings(const QVideoEncoderSettings&);

	QImageEncoderSettings imageSettings() const;
	void setImageSettings(const QImageEncoderSettings &settings);
*/
	QString format() const;
	void setFormat(const QString &format);

protected:
	void changeEvent(QEvent *e);

private:
	QVariant boxValue(const QComboBox*) const;
	void selectComboBoxItem(QComboBox *box, const QVariant &value);

};

#endif
// CAMERASETTINGS_H
