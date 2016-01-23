#ifndef REMOTE_HPP
#define REMOTE_HPP

#include "sensory/SensorInput.hpp"
#include "comms/CommsChannel.hpp"
#include "comms/messages/StatusMessage.hpp"

#include <QObject>

class Remote : public QObject{
		Q_OBJECT
	private:

		SensorInput sensors;
		CommsChannel *server;
		StatusMessage statusMessage;
		qint64 lastSend;

	public:
		explicit Remote(QObject *parent = 0);
		virtual ~Remote();

		void hookSignals(QObject *o);
		void unHookSignals(QObject *o);
	private:

		void sendStatus();

	private slots:

		void onServerReceivePacket(QByteArray,QHostAddress,quint16);
		void onServerError(QString);

	private slots:
		void onPositionUpdated(const QGeoPositionInfo &info);
		void onCompassUpdated(QCompassReading *);
		void onAccelerometerUpdated(QAccelerometerReading *);
		void onGyroscopeUpdated(QGyroscopeReading *r);


};

#endif // REMOTE_HPP
