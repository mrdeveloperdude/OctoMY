#ifndef AGENT_HPP
#define AGENT_HPP

#include "sensory/SensorInput.hpp"
#include "comms/CommsChannel.hpp"
#include "comms/messages/SensorsMessage.hpp"

#include <QObject>
#include <QCommandLineParser>


class Agent : public QObject{
		Q_OBJECT
	private:

		QCommandLineParser &opts;
		CommsChannel *comms;
		SensorInput sensors;
		SensorsMessage statusMessage;
		qint64 lastSend;

		QString hubAddress;
		quint16 hubPort;


	public:
		explicit Agent(QCommandLineParser &opts, QObject *parent = 0);
		virtual ~Agent();

		void start(QString adrLocal, quint16 portLocal, QString hubAddress, quint16 hubPort);
		void hookSignals(QObject &o);
		void unHookSignals(QObject &o);
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



#endif // AGENT_HPP
