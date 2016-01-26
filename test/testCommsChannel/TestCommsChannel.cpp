#include "TestCommsChannel.hpp"

#include "comms/CommsChannel.hpp"
#include "basic/Standard.hpp"

#include <QSignalSpy>

CommsTester::CommsTester(QString name, QHostAddress adr, quint16 port):
	name(name)
  , adr(adr)
  , port(port)
  , cc(new CommsChannel(0))
{
	QVERIFY(0!=cc);
	if(0!=cc){
		qRegisterMetaType<QHostAddress>("QHostAddress");
		qRegisterMetaType<QByteArray>("QByteArray");
		if(!connect(cc,SIGNAL(receivePacket(QByteArray,QHostAddress,quint16)),this, SLOT(onReceivePacket(QByteArray,QHostAddress,quint16)),WWCONTYPE)){
			qDebug()<<"could not connect";
		}
		if(!connect(cc,SIGNAL(error(QString)),this,SLOT(onError(QString)),WWCONTYPE)){
			qDebug()<<"could not connect";
		}
		cc->start(adr,port);
	}
}

void CommsTester::onReceivePacket(QByteArray datagram,QHostAddress adr,quint16 p){
	qDebug()<<toString()<<": onReceivePacket got '"<<datagram<<"'' from "<<adr<<":"<<p;
}

void CommsTester::onError(QString e){
	qDebug()<<toString()<<": onError got "<<e;
}


void CommsTester::startSendTest(CommsTester &cc2){
	QByteArray datagram="PING\0";
	QVERIFY(port!=cc2.port);
	qRegisterMetaType<QHostAddress>("QHostAddress");
	qRegisterMetaType<QByteArray>("QByteArray");
	for(int i=0,delay=1000;delay>0;delay=1000/++i){
		qDebug()<<"\n\n------ testing with delay:"<<delay << " and summary: " <<cc2.cc->getSummary();
		//TODO: Convert this to use couriers
		//cc->sendPackage(datagram,cc2.adr,cc2.port);
		QTest::qWait(delay);
	}
	QTest::qWait(4000);
}


QString CommsTester::toString(){
	QString ret=name;
	ret+="(";
	ret+=adr.toString();
	ret+=":";
	ret+=QString::number(port);
	ret+=")";
	return ret;
}


// http://doc.qt.io/qt-5/qtest.html
void TestCommsChannel::test(){
	QHostAddress local("127.0.0.1");
	quint16 port1=54345;
	quint16 port2=port1+1;
	cc1=new CommsTester("test_server",local, port1);
	QVERIFY(0!=cc1);
	cc2=new CommsTester("test_client", local, port2);
	QVERIFY(0!=cc2);
	cc1->startSendTest(*cc2);
}



QTEST_MAIN(TestCommsChannel)
