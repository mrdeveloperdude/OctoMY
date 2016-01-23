#ifndef COMMSCHANNEL_HPP
#define COMMSCHANNEL_HPP

#include <QObject>
#include <QHostAddress>
#include <QUdpSocket>
#include <QList>
#include <QTimer>
#include <QDateTime>

#include "ReliabilitySystem.hpp"
#include "FlowControl.hpp"

#include "basic/LogDestination.hpp"

#define TIMEOUT_TRESHOLD 1

class HubWindow;
class Client;

class CommsChannel : public QObject{
		Q_OBJECT
	private:
		QUdpSocket udpSocket;
		QHostAddress localAddress;
		quint16 localPort;
		QTimer idleTimer;
		QMap<quint64, Client *> clients;
		LogDestination *mw;


	public:

		explicit CommsChannel(LogDestination *mw=0);

	public:

		void start(QHostAddress localAddress, quint16 localPort);
		void stop();


		void setLogOutput(LogDestination *mw);
		bool sendPackage(QByteArray ba,QHostAddress host, quint16 port);
		QString getSummary();

		QMap<quint64, Client *> &getClients ();

		void hookSignals(QObject &ob);
		void unHookSignals(QObject &ob);


	private:
		void appendLog(QString);
		Client *getClient(QHostAddress host, quint16 port);


	signals:

		void receivePacket(QSharedPointer<QDataStream> data,QHostAddress host, quint16 port);
		void error(QString message);
		void clientAdded(Client *c);

	private slots:

		void receivePacketRaw(QByteArray ba,QHostAddress host, quint16 port);
		void onIdleTimer();

	public slots:

		void onReadyRead();
		void onUdpError(QAbstractSocket::SocketError);
};

#endif // COMMSCHANNEL_HPP
