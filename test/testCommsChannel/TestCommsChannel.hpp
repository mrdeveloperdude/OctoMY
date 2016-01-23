#ifndef TESTCOMMSCHANNEL_HPP
#define TESTCOMMSCHANNEL_HPP

#include <QTest>
#include <QHostAddress>

class CommsChannel;

class CommsTester:public QObject{
		Q_OBJECT

	private:
		QString name;
		QHostAddress adr;
		quint16 port;
		CommsChannel *cc;


	public:

		CommsTester(QString name, QHostAddress adr, quint16 port);
		void startSendTest(CommsTester &cc2);
		QString toString();

	public slots:
		void onReceivePacket(QByteArray,QHostAddress,quint16);
		void onError(QString);


};

class TestCommsChannel:public QObject{
		Q_OBJECT
	private:
		CommsTester *cc1;
		CommsTester *cc2;

	private slots:
		void test();

};


#endif // TESTCOMMSCHANNEL_HPP
