#include "TestCommsChannel.hpp"

#include "../../libs/libcore/basic/Standard.hpp"
#include "../../libs/libcore/comms/messages/MessageType.hpp"
#include "random/RNG.hpp"

#include <QSignalSpy>


TestCourier::TestCourier(ClientSignature dest, CommsTester *parent, const qint32 maxSends , const qint32 maxRecs)
	: Courier("Test", Courier::FIRST_USER_ID+313, parent)
	, ct(parent)
	, soFar(0)
	, datagram("LOREM IPSUM DOLOR SIT AMET")
	, maxSends(maxSends)
	, maxRecs(maxRecs)
	, sendCount(0)
	, recCount(0)
	, man(datagram.size()+4,10,100,true,true)
{
	setDestination(dest);
}

TestCourier::~TestCourier(){

}



//Let the CommChannel know what we want
CourierMandate TestCourier::mandate(){
	return man;
}

//Override to act on sending opportunity.
//Return number of bytes sent ( >0 ) if you took advantage of the opportunity
quint16 TestCourier::sendingOpportunity(QDataStream &ds ){
	sendCount++;
	if(sendCount>=maxSends){
		qDebug()<<"MAX SENDS ("<<maxSends<<") RECEIVED FOR "<<getDestination().toString()<<" STOPPING";
		man.sendActive=false;
	}
	ds << datagram;
	const bool done=soFar++>=ct->testCount;
	qDebug()<<"SENDING OPPORTUNITY: "<<getDestination().toString()<< man.payloadSize<<(done?"DONE":"");
	if(done){
		emit ct->finished();
	}
	return man.payloadSize;
}

//Override to act on data received
//Return number of bytes actually read.
quint16 TestCourier::dataReceived(QDataStream &ds, quint16 availableBytes){
	recCount++;
	if(recCount>=maxRecs){
		qDebug()<<"MAX RECS ("<<maxRecs<<") RECEIVED FOR "<<getDestination().toString()<<" STOPPING";
		man.receiveActive=false;
	}
	QByteArray in;
	ds>>in;
	qDebug()<<"DATA BYTES RECEIVED="<<availableBytes<<" ("<<in<<")";
	return in.size()+4;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

CommsTester::CommsTester(QString name, QHostAddress myAddress, quint16 myPort, quint16 basePort, quint16 portRange, quint16 testCount, QObject *parent)
	: QObject(parent)
	, name(name)
	, myAddress(myAddress)
	, myPort(myPort)
	, basePort(basePort)
	, portRange(portRange)
	, testCount(testCount)
	, rng(RNG::sourceFactory("mt"))
{
	QVERIFY(nullptr!=rng);
	rng->init(myPort);
	QVERIFY(myPort>=basePort);
	QVERIFY(myPort<=basePort+portRange);
	QVERIFY(connect(&cc,SIGNAL(error(QString)),this,SLOT(onError(QString)),OC_CONTYPE));
	for(quint16 toPort=basePort;toPort<basePort+portRange;++toPort){
		if(myPort==toPort){
			continue;
		}
		if(rng->generateReal2()>0.7 || true){
			qDebug() << myAddress << ":" << myPort << " --> " << toPort;
			ClientSignature sig(0,0,myAddress, toPort);
			TestCourier *tc=new TestCourier(sig, this, testCount, testCount);
			QVERIFY(nullptr!=tc);
			cc.registerCourier(*tc);
		}
		else{
			qDebug() << myAddress << ":" << myPort << " --> SKIPPED";
		}
	}
}


void CommsTester::onError(QString e){
	qWarning()<<toString()<<"::onError(): "<<e;
}



void CommsTester::onReadyRead(){
	qDebug()<<toString()<<"::onReadyRead()";
}


void CommsTester::startSendTest(){
	qDebug()<<"Starting test for "<<toString();
	cc.start(myAddress, myPort);
}


QString CommsTester::toString(){
	QString ret=name;
	ret+="(";
	ret+=myAddress.toString();
	ret+=":";
	ret+=QString::number(myPort);
	ret+=")";
	return ret;
}


/*
Sets up X commchannels and have them sending data
via courier to a random but deterministically selected subset of X.

Result should stress the UDP system in a way that lays bare any shortcomings
when used in real world scenario.

TODO: add simulated unreliability factors such as dropped/corrupted/delayed packets
*/
// http://doc.qt.io/qt-5/qtest.html
void TestCommsChannel::test(){
	qRegisterMetaType<QHostAddress>("QHostAddress");
	qRegisterMetaType<QByteArray>("QByteArray");
	const QHostAddress local("127.0.0.1");
	const quint16 basePort=54345;
	const quint16 portRange=5;
	const quint16 testCount=3;
	QList<CommsTester *> senders;
	qDebug()<<"####################################### INITIALIZING";
	for(quint16 i=0;i<portRange;++i){
		const quint16 tport=basePort+i;
		CommsTester *cc=new CommsTester("instance_"+QString::number(tport), local, tport, basePort, portRange, testCount);
		QVERIFY(nullptr!=cc);
		senders.push_back(cc);
	}
	qDebug()<<"####################################### STARTING";
	for(quint16 i=0;i<portRange;++i){
		//const quint16 tport=basePort+1+i;
		QVERIFY(QMetaObject::invokeMethod(senders[i], "startSendTest",  Qt::QueuedConnection));
	}
	qDebug()<<"####################################### WAITING";
	for(quint16 i=0;i<portRange;++i){
		QSignalSpy spy(senders[i],SIGNAL(finished()));
		QVERIFY(spy.wait());
	}
	const quint64 start=QDateTime::currentMSecsSinceEpoch();
	quint64 now=start;
	while(now<start+3000){;
		now=QDateTime::currentMSecsSinceEpoch();
		QCoreApplication::processEvents();
	}
	qDebug()<<"####################################### DELETING";
	for(quint16 i=0;i<portRange;++i){
		senders[i]->deleteLater();
	}
	senders.clear();
}



QTEST_MAIN(TestCommsChannel)
