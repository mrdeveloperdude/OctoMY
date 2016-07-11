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
class AppContext;

class Node : public QObject
{
		Q_OBJECT
	protected:

		AppContext *mContext;
		KeyStore keystore;
		DiscoveryClientStore peers;

		DiscoveryClient *discovery;
		DiscoveryRole role;
		DiscoveryType type;
		CommsChannel *comms;
		ZooClient *zoo;
		SensorInput *sensors;

		NetworkAddress controlAddress;
		CameraList *cameras;

		qint64 lastStatusSend;
		SensorsMessage *sensorMessage;


	public:
		explicit Node(AppContext *context, DiscoveryRole role, DiscoveryType type, QObject *parent = nullptr);
		virtual ~Node();

	public:

		void updateDiscoveryClient();
		void sendStatus();

		void hookSensorSignals(QObject &o);
		void unHookSensorSignals(QObject &o);

		void hookCommsSignals(QObject &o);
		void unHookCommsSignals(QObject &o);

		const QCommandLineParser &getOptions() const;
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

		// KeyStore slots
	private slots:
		void onKeystoreReady(bool);

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
