#ifndef CAMERA_H
#define CAMERA_H




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

class Camera : public QWidget{
		Q_OBJECT

	private:
		Ui::Camera *ui;
		QTimer devChangeTimer;
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
		QString camListHash;


	public:
		Camera(QWidget *parent = 0);
		~Camera();

	private slots:
		void setCamera(const QCameraInfo &cameraInfo);

		void onDevChangeTimer();

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

		void displayRecorderError();
		void displayCameraError();

		void updateCameraDevice(QAction *action);

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

		void on_pushButtonToggleCamera_clicked();

		void on_pushButtonSettings_clicked();

		void showMessage(QString msg);

		void detectBarcodes(const QVideoFrame &);

	protected:
		void keyPressEvent(QKeyEvent *event);
		void keyReleaseEvent(QKeyEvent *event);
		void closeEvent(QCloseEvent *event);

	signals:

		void cameraDevicesChanged();

};

#endif
