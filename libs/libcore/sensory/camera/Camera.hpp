#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <QObject>
#include <QImageEncoderSettings>
#include <QAudioEncoderSettings>
#include <QVideoEncoderSettings>
#include <QCameraInfo>
#include <QMediaRecorder>
#include <QCameraImageCapture>


class QCamera;

class Camera : public QObject{
		Q_OBJECT

	private:

		QCamera *camera;
		QCameraImageCapture *imageCapture;
		QMediaRecorder* mediaRecorder;

		QImageEncoderSettings imageSettings;
		QAudioEncoderSettings audioSettings;
		QVideoEncoderSettings videoSettings;
		QString videoContainerFormat;
		bool isCapturingImage;
		bool applicationExiting;


	public:
		explicit Camera(QObject *parent = 0);
		virtual ~Camera();

	private slots:
		void setCamera(const QCameraInfo &cameraInfo);

		void startCamera();
		void stopCamera();

		void record();
		void pause();
		void stop();
		void setMuted(bool);

		void toggleLock();
		void takeImage();
		void displayCaptureError(int, QCameraImageCapture::Error, const QString &errorString);

		void configureCaptureSettings();
		void configureVideoSettings();
		void configureImageSettings();

		void displayRecorderError();
		void displayCameraError();

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


};

#endif // CAMERA_HPP
