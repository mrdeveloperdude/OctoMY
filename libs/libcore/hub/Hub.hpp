#ifndef HUB_HPP
#define HUB_HPP

#include "comms/CommsChannel.hpp"
#include "security/KeyStore.hpp"
#include "zoo/ZooClient.hpp"
#include <QCommandLineParser>

class Hub: QObject{
		Q_OBJECT
	private:
		QCommandLineParser &opts;
		CommsChannel *comms;
		ZooClient *zoo;
		int lastSentPackets;
		int lastReceivedPackets;
		int lastLostPackets;
		int lastAckedPackets;
		KeyStore keystore;


	public:

		explicit Hub(QCommandLineParser &opts, QObject *parent = 0);
		virtual ~Hub();


		void hookSignals(QObject *o);
		void unHookSignals(QObject *o);

		QCommandLineParser &getOptions();
		CommsChannel *getComms();

		ZooClient *getZoo();

	private slots:

		void onReceivePacket(QSharedPointer<QDataStream>,QHostAddress,quint16);
		void onError(QString);
		void onClientAdded(Client *);

	public slots:
		void onConnectionStatusChanged(bool s);

};

#endif // HUB_HPP
