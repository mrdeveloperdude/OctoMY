#ifndef NODE_HPP
#define NODE_HPP


#include "basic/Settings.hpp"
#include "security/KeyStore.hpp"

#include <QObject>
#include <QHostAddress>

class CommsChannel;
class ZooClient;
class DiscoveryClient;

class SensorInput;
class CameraList;
class SensorsMessage;
class QCommandLineParser;

class QGeoPositionInfo;
class QCompassReading;
class QAccelerometerReading;
class QGyroscopeReading;
class Client;


class Node : public QObject
{
		Q_OBJECT
	protected:

		QCommandLineParser &opts;
		Settings settings;
		KeyStore keystore;

		DiscoveryClient *discovery;
		CommsChannel *comms;
		ZooClient *zoo;
		SensorInput *sensors;

		QHostAddress hubAddress;
		quint16 hubPort;
		CameraList *cameras;

		qint64 lastStatusSend;
		SensorsMessage *sensorMessage;



	public:
		explicit Node(QCommandLineParser &opts, QString base, QObject *parent = nullptr);


	public:

		void sendStatus();

		void hookSensorSignals(QObject &o);
		void unHookSensorSignals(QObject &o);

		QCommandLineParser &getOptions();
		Settings &getSettings();
		KeyStore  &getKeyStore();

		DiscoveryClient *getDiscoveryClient();
		CommsChannel *getComms();
		ZooClient *getZooClient();
		SensorInput *getSensorInput();

		CameraList *getCameras();

		virtual QWidget *showWindow();



		// CommsChannel slots
	private slots:
		void onReceivePacket(QSharedPointer<QDataStream>,QHostAddress,quint16);
		void onError(QString);
		void onClientAdded(Client *);
		void onConnectionStatusChanged(bool);


		// SensorInput slots
	private slots:
		void onPositionUpdated(const QGeoPositionInfo &info);
		void onCompassUpdated(QCompassReading *);
		void onAccelerometerUpdated(QAccelerometerReading *);
		void onGyroscopeUpdated(QGyroscopeReading *r);



};

#endif // NODE_HPP
