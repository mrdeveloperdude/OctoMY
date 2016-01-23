#ifndef REMOTE_HPP
#define REMOTE_HPP

#include "sensory/SensorInput.hpp"
#include "comms/CommsChannel.hpp"
#include "comms/messages/StatusMessage.hpp"

#include <QObject>
#include <QCommandLineParser>


class Remote : public QObject{
		Q_OBJECT
	private:

		QCommandLineParser &opts;
		CommsChannel *comms;
		SensorInput sensors;
		StatusMessage statusMessage;
		qint64 lastSend;

		QString hubAddress;
		quint16 hubPort;


	public:
		explicit Remote(QCommandLineParser &opts, QObject *parent = 0);
		virtual ~Remote();

		void start(QString adrLocal, quint16 portLocal, QString hubAddress, quint16 hubPort);
		void hookSignals(QObject *o);
		void unHookSignals(QObject *o);
		void sendStatus();

	private slots:

		void onReceivePacket(QSharedPointer<QDataStream>,QHostAddress,quint16);
		void onError(QString);
		void onClientAdded(Client *);


	private slots:
		void onPositionUpdated(const QGeoPositionInfo &info);
		void onCompassUpdated(QCompassReading *);
		void onAccelerometerUpdated(QAccelerometerReading *);
		void onGyroscopeUpdated(QGyroscopeReading *r);


};

#endif // REMOTE_HPP
