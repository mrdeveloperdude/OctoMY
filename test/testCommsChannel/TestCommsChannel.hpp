#ifndef TESTCOMMSCHANNEL_HPP
#define TESTCOMMSCHANNEL_HPP

#include "../../libs/libcore/comms/CommsChannel.hpp"


#include <QTest>
#include <QHostAddress>

class CommsTester;
class TestCourier:public Courier{
		Q_OBJECT

	public:

		CommsTester *ct;
		quint16 soFar;

		const QByteArray datagram;
		const qint32 maxSends;
		const qint32 maxRecs;
		qint32 sendCount;
		qint32 recCount;
		CourierMandate man;

	public:
		explicit TestCourier(ClientSignature dest, CommsTester *parent = nullptr, const qint32 maxSends=1 , const qint32 maxRecs=1 );
		virtual ~TestCourier();

	public:

		//Let the CommChannel know what we want
		virtual CourierMandate mandate();

		//Override to act on sending opportunity.
		//Return nubmer of bytes sent ( >0 ) if you took advantage of the opportunity
		quint16 sendingOpportunity(QDataStream &ds) override;

		//Override to act on data received
		//Return number of bytes actually read.
		quint16 dataReceived(QDataStream &ds, quint16 availableBytes) override;

};



class RNG;
class CommsTester:public QObject{
		Q_OBJECT

	public:
		QString name;
		QHostAddress myAddress;
		quint16 myPort;


		quint16 basePort;
		quint16 portRange;
		CommsChannel cc;
		quint16 testCount;

		RNG *rng;


	public:
		explicit CommsTester(QString name, QHostAddress myAddress, quint16 myPort, quint16 basePort, quint16 portRange, quint16 testCount, QObject *parent=nullptr);
		virtual ~CommsTester(){}
		QString toString();

	public slots:
		void startSendTest();
		void onError(QString);
		void onReadyRead();
	signals:
		void finished();

};

class TestCommsChannel:public QObject{
		Q_OBJECT
	private slots:
		void test();

};


#endif // TESTCOMMSCHANNEL_HPP
