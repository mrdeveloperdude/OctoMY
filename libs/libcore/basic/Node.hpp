#ifndef NODE_HPP
#define NODE_HPP

#include "comms/discovery/DiscoveryRole.hpp"

#include "basic/Settings.hpp"
#include "security/KeyStore.hpp"
#include "comms/discovery/DiscoveryClientStore.hpp"

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
		QString baseDir;
		KeyStore keystore;
		DiscoveryClientStore peers;

		DiscoveryClient *discovery;
		DiscoveryRole role;
		DiscoveryType type;
		CommsChannel *comms;
		ZooClient *zoo;
		SensorInput *sensors;

		QHostAddress hubAddress;
		quint16 hubPort;
		CameraList *cameras;

		qint64 lastStatusSend;
		SensorsMessage *sensorMessage;



	public:
		explicit Node(QCommandLineParser &opts, QString base, DiscoveryRole role, DiscoveryType type, QObject *parent = nullptr);
		virtual ~Node();

	public:

		void sendStatus();

		void hookSensorSignals(QObject &o);
		void unHookSensorSignals(QObject &o);

		void hookCommsSignals(QObject &o);
		void unHookCommsSignals(QObject &o);

		QCommandLineParser &getOptions();
		Settings &getSettings();
		KeyStore  &getKeyStore();
		DiscoveryClientStore &getPeers();




		DiscoveryClient *getDiscoveryClient();
		DiscoveryRole getRole();
		DiscoveryType getType();
		CommsChannel *getComms();
		ZooClient *getZooClient();
		SensorInput *getSensorInput();

		CameraList *getCameras();

		virtual QWidget *showWindow();



		// CommsChannel slots
	private slots:
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
