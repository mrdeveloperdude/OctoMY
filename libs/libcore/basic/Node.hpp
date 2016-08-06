#ifndef NODE_HPP
#define NODE_HPP

#include "comms/discovery/DiscoveryRole.hpp"

#include "basic/Settings.hpp"
#include "security/KeyStore.hpp"
#include "comms/discovery/NodeAssociateStore.hpp"

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
		KeyStore mKeystore;
		NodeAssociateStore mPeers;

		DiscoveryClient *mDiscovery;
		DiscoveryRole mRole;
		DiscoveryType mType;
		CommsChannel *mComms;
		ZooClient *mZooClient;
		SensorInput *mSensors;

		NetworkAddress mPartnerAddress;
		CameraList *mCameras;

		qint64 mLastStatusSend;
		SensorsMessage *mSensorMessage;


	public:
		explicit Node(AppContext *context, DiscoveryRole mRole, DiscoveryType mType, QObject *parent = nullptr);
		virtual ~Node();

	public:

		void updateDiscoveryClient();
		void sendStatus();

		void hookSensorSignals(QObject &o);
		void unHookSensorSignals(QObject &o);

		void hookCommsSignals(QObject &o);
		void unHookCommsSignals(QObject &o);

		void hookPeerSignals(QObject &o);
		void unHookPeerSignals(QObject &o);

		const QCommandLineParser &options() const;
		Settings &settings();
		KeyStore  &keyStore();
		NodeAssociateStore &peers();
		DiscoveryClient *discoveryClient();
		DiscoveryRole role();
		DiscoveryType type();
		QString name();
		CommsChannel *comms();
		ZooClient *zooClient();
		SensorInput *sensorInput();

		QSharedPointer<NodeAssociate> localNodeAssociate();

		CameraList *cameras();

		virtual QWidget *showWindow();

	signals:


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
