#ifndef REMOTE_HPP
#define REMOTE_HPP

#include "sensory/SensorInput.hpp"
#include "comms/CommsChannel.hpp"
#include "comms/messages/SensorsMessage.hpp"

#include "comms/couriers/DirectPoseCourier.hpp"

#include <QObject>
#include <QCommandLineParser>
#include <QVector2D>


class Remote : public QObject{
		Q_OBJECT
	private:

		QCommandLineParser &opts;
		CommsChannel *comms;
		SensorInput sensors;
		SensorsMessage statusMessage;
		qint64 lastSend;

		QHostAddress hubAddress;
		quint16 hubPort;
		DirectPoseCourier *poseCourier;


	public:
		explicit Remote(QCommandLineParser &opts, QObject *parent = 0);
		virtual ~Remote();

		void start(QHostAddress adrLocal, quint16 portLocal, QHostAddress hubAddress, quint16 hubPort);
		void hookSignals(QObject &);
		void unHookSignals(QObject &);


		//Internal network events
	private slots:

		void onReceivePacket(QSharedPointer<QDataStream>,QHostAddress,quint16);
		void onError(QString);
		void onClientAdded(Client *);
		void onConnectionStatusChanged(bool s);

		//External commands
	public slots:


		void onDirectPoseChanged(Pose);

		//Internal sensors
	private slots:


		void onPositionUpdated(const QGeoPositionInfo &info);
		void onCompassUpdated(QCompassReading *);
		void onAccelerometerUpdated(QAccelerometerReading *);
		void onGyroscopeUpdated(QGyroscopeReading *r);

		//External sensors
	public slots:
		void onTouchUpdated(QVector2D);

};

#endif // REMOTE_HPP
