#ifndef CAMERALIST_HPP
#define CAMERALIST_HPP


#include <QTimer>
#include <QList>
#include <QCameraDevice>




class CameraList: public QObject
{
	Q_OBJECT

private:
	QTimer mDevChangeTimer;
	QString mDeviceListHash;
	QList<QCameraDevice> mDeviceList;

public:
	CameraList(QObject *parent=0);

public:
	void setUpDeviceTimer(const quint64 ms=5000);
	QString toSpecStanzas(QString space="");

private:
	QString deviceListToHash(QList<QCameraDevice> devices);

private slots:
	void onDevChangeTimer();

signals:
	void cameraDevicesChanged();

};

#endif
// CAMERALIST_HPP
