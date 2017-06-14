#ifndef SERIALLIST_HPP
#define SERIALLIST_HPP

#include <QObject>
#include <QTimer>
#include <QSerialPortInfo>

class SerialList : public QObject
{
		Q_OBJECT
	private:
		QTimer mDeviceChangeTimer;
		QString mDeviceListHash;
		QList<QSerialPortInfo> mDeviceList;

	public:
		SerialList(QObject *parent=0);
	public:
		void setUpDeviceTimer(const quint64 ms=5000);
		QString toSpecStanzas(QString space="");

		int count();

		QSerialPortInfo device(int index);

	private:
		QString deviceListToHash(QList<QSerialPortInfo> devices);

	private slots:
		void onDevChangeTimer();

	signals:
		void serialDevicesChanged();

};

#endif // SERIALLIST_HPP
