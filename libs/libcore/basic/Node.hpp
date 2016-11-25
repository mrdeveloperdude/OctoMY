#ifndef NODE_HPP
#define NODE_HPP

#include "discovery/DiscoveryRole.hpp"
#include "discovery/NodeAssociateStore.hpp"

#include "basic/Settings.hpp"
#include "security/KeyStore.hpp"

#include <QObject>
#include <QHostAddress>
#include <QUrl>

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
class SensorsCourier;
class BlobCourier;
class BlobFuture;

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

	SensorsCourier *mSensorsCourier;
	BlobCourier *mBlobCourier;

	//NetworkAddress mPartnerAddress;
	CameraList *mCameras;

	qint64 mLastStatusSend;
//	SensorsMessage *mSensorMessage;

	QUrl mServerURL;


public:
	explicit Node(AppContext *context, DiscoveryRole mRole, DiscoveryType mType, QObject *parent = nullptr);
	virtual ~Node();

public:

	void init();

	void updateDiscoveryClient();

	void hookColorSignals(QObject &o);
	void unHookColorSignals(QObject &o);

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

	QSharedPointer<NodeAssociate> nodeIdentity();

	CameraList *cameras();

	virtual QWidget *showWindow();

	void start(const NetworkAddress &localAddress);
	void stop();

	// Blob courier
	BlobFuture submitBlobForSending(QByteArray data, QString name);


	// Blob courier
signals:
	void blobReceived(QString name);
	void blobSendComplete(QString name);

	// KeyStore slots
private slots:
	void onKeystoreReady(bool);

	// CommsChannel slots
private slots:
	void onCommsError(QString);
	void onCommsClientAdded(Client *);
	void onCommsConnectionStatusChanged(bool);


	// SensorInput slots
private slots:
	void onPositionUpdated(const QGeoPositionInfo &info);
	void onCompassUpdated(QCompassReading *);
	void onAccelerometerUpdated(QAccelerometerReading *);
	void onGyroscopeUpdated(QGyroscopeReading *r);

};

#endif // NODE_HPP
