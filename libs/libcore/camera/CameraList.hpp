#ifndef CAMERALIST_HPP
#define CAMERALIST_HPP


#include <QTimer>
#include <QCameraInfo>



class CameraList: public QObject
{
		Q_OBJECT
	private:
		QTimer devChangeTimer;
		QString camListHash;
		QList<QCameraInfo>  cams;

	public:
		CameraList(QObject *parent=0);
	public:
		void setUpDeviceTimer(const quint64 ms=5000);
		QString toSpecStanzas(QString space="");

	private:
		QString cameraListToHash(QList<QCameraInfo> cameras);


	private slots:

		void onDevChangeTimer();

	signals:

		void cameraDevicesChanged();

};

#endif // CAMERALIST_HPP
