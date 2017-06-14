#ifndef CAMERA_H
#define CAMERA_H

#include "CameraList.hpp"

#include <QCameraImageCapture>
#include <QMediaRecorder>

#include <QCameraInfo>
#include <QWidget>
#include <QVideoProbe>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class Camera; }
QT_END_NAMESPACE


class PoorMansProbe;
class ZBarScanner;
class QActionGroup;
class QCamera;
class QCameraInfo;
class QComboBox;
class QListWidgetItem;

class Camera : public QWidget{
		Q_OBJECT

	private:
		Ui::Camera *ui;
		QCamera *camera;
		QCameraInfo cameraInfo;
		QCameraImageCapture *imageCapture;
		QMediaRecorder* mediaRecorder;
		QVideoProbe *videoProbe;
		PoorMansProbe *poorVideoProbe;
		ZBarScanner *zbar;

		QImageEncoderSettings imageSettings;
		QAudioEncoderSettings audioSettings;
		QVideoEncoderSettings videoSettings;
		QString videoContainerFormat;
		bool isCapturingImage;
		bool applicationExiting;
		QActionGroup *videoDevicesGroup;
		CameraList cl;


	public:
		Camera(QWidget *parent = 0);
		~Camera();

	private slots:
		void setCamera(const QCameraInfo &cameraInfo);

		void onCameraDevicesChanged();

		void startCamera();
		void stopCamera();

		void record();
		void pause();
		void stop();
		void setMuted(bool);

		void tryFocus();
		void captureImage();
		void displayCaptureError(int, QCameraImageCapture::Error, const QString &errorString);

		void configureCaptureSettings();

		void displayRecorderError();
		void displayCameraError();

		void updateCameraDevice(QListWidgetItem *action);

		void updateCameraState(QCamera::State);
		void updateCaptureMode();
		void updateRecorderState(QMediaRecorder::State state);
		void setExposureCompensation(int index);

		void updateRecordTime();

		void processCapturedImage(int requestId, const QImage &img);
		void updateLockStatus(QCamera::LockStatus, QCamera::LockChangeReason);

		void displayViewfinder();
		void displayCapturedImage();

		void readyForCapture(bool ready);
		void imageSaved(int id, const QString &fileName);

		void showMessage(QString msg);

		void detectBarcodes(const QVideoFrame &);

		//////////////////////////////////

		void on_toolButtonCaptureImage_clicked();

		void on_toolButtonRecordVideo_toggled(bool checked);

		void on_toolButtonBack_clicked();

		void on_toolButtonSettings_clicked();

		void on_toolButtonFocus_clicked();

	protected:
		void keyPressEvent(QKeyEvent *event);
		void keyReleaseEvent(QKeyEvent *event);
		void closeEvent(QCloseEvent *event);


		///

	public:


		QAudioEncoderSettings getAudioSettings() const;
		void setAudioSettings(const QAudioEncoderSettings&);

		QVideoEncoderSettings getVideoSettings() const;
		void setVideoSettings(const QVideoEncoderSettings&);

		QImageEncoderSettings getImageSettings() const;
		void setImageSettings(const QImageEncoderSettings &settings);

		QString format() const;
		void setFormat(const QString &format);


	private:
		QVariant boxValue(const QComboBox*) const;
		void selectComboBoxItem(QComboBox *box, const QVariant &value);

};

#endif
