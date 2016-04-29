#ifndef SERIALLIST_HPP
#define SERIALLIST_HPP

#include <QObject>
#include <QTimer>
#include <QSerialPortInfo>

class SerialList : public QObject
{
		Q_OBJECT
	private:
		QTimer deviceChangeTimer;
		QString deviceListHash;
		QList<QSerialPortInfo> deviceList;

	public:
		SerialList(QObject *parent=0);
	public:
		void setUpDeviceTimer(const quint64 ms=5000);
		QString toSpecStanzas(QString space="");

	private:
		QString deviceListToHash(QList<QSerialPortInfo> devices);

	private slots:
		void onDevChangeTimer();

	signals:
		void serialDevicesChanged();

};

#endif // SERIALLIST_HPP
