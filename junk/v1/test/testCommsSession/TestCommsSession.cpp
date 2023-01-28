#include "TestCommsSession.hpp"

#include "comms/CommsSession.hpp"
#include "security/Key.hpp"
#include "utility/Standard.hpp"


void TestCommsSession::test()
{

	QSharedPointer<Key> key(OC_NEW Key(128));
	QVERIFY(nullptr!=key);

	{
		/////////////////////////////////////////////////
		// explicit CommsSession(Key &key);

		CommsSession sess(key);

		/////////////////////////////////////////////////
		// virtual ~CommsSession();

	}


	CommsSession sess(key);

	/////////////////////////////////////////////////
	// ReliabilitySystem &reliabilitySystem();
	{
		ReliabilitySystem &rs=sess.reliabilitySystem();
	}

	/////////////////////////////////////////////////
	// FlowControl &flowControl();
	{
		FlowControl &fc=sess.flowControl();
	}

	/////////////////////////////////////////////////
	// qint64 lastSendTime() const;
	{
		qint64 lst=sess.lastSendTime();
	}


	/////////////////////////////////////////////////
	// qint64 lastRecieveTime() const;
	{
		qint64 lrt=sess.lastRecieveTime();
	}

	/////////////////////////////////////////////////
	// float timeoutAccumulator() const;
	{
		float toa=sess.timeoutAccumulator();
	}

	/////////////////////////////////////////////////
	// float deltaTime() const;
	{
		float dt=sess.deltaTime();
	}

	/////////////////////////////////////////////////
	// float idleAccumulator() const;
	{
		float ia=sess.idleAccumulator();
	}

	/////////////////////////////////////////////////
	// quint32 idlePacksSent() const;
	{
		float ips=sess.idlePacksSent();
	}

	/////////////////////////////////////////////////
	// Key &key() const;
	{
		QSharedPointer<Key> k2=sess.key();
		QCOMPARE(key,k2);
	}

	/////////////////////////////////////////////////
	// bool connected() const;
	{
		bool c=sess.connected();
	}

	/////////////////////////////////////////////////
	// bool lastConnected() const;
	{
		bool lc=sess.lastConnected();
	}


	/////////////////////////////////////////////////
	// bool established() const;
	{
		bool e=sess.established();
	}


	/////////////////////////////////////////////////
	// HandshakeStep nextStep() const;
	{
		//HandshakeStep ns=sess.nextStep();
	}


	/////////////////////////////////////////////////
	// bool expired() const;

	/////////////////////////////////////////////////
	// void setExpired();
	{
		bool e1=sess.expired();
		QVERIFY(!e1);
		sess.setExpired();
		bool e2=sess.expired();
		QVERIFY(e2);
	}

	/////////////////////////////////////////////////
	// SESSION_ID_TYPE localSessionID() const;

	/////////////////////////////////////////////////
	// void setLocalSessionID(SESSION_ID_TYPE );
	{
		const SESSION_ID_TYPE lsid1=sess.localSessionID();
		const SESSION_ID_TYPE localSessID=12345;
		QVERIFY(lsid1!=localSessID);
		sess.setLocalSessionID(localSessID);
		const SESSION_ID_TYPE lsid2=sess.localSessionID();
		QVERIFY(lsid2==localSessID);
	}


	/////////////////////////////////////////////////
	// SESSION_ID_TYPE remoteSessionID() const;

	/////////////////////////////////////////////////
	// void setRemoteSessionID(SESSION_ID_TYPE );
	{
		const SESSION_ID_TYPE rsid1=sess.remoteSessionID();
		const SESSION_ID_TYPE remoteSessID=12345;
		QVERIFY(rsid1!=remoteSessID);
		sess.setRemoteSessionID(remoteSessID);
		const SESSION_ID_TYPE rsid2=sess.remoteSessionID();
		QVERIFY(rsid2==remoteSessID);
	}


	/////////////////////////////////////////////////
	// QString fullID() const;
	{
		QString fid=sess.fullID();
		QVERIFY(fid==key->id());
	}


	/////////////////////////////////////////////////
	// NetworkAddress address() const;
	/////////////////////////////////////////////////
	// void setAddress(const NetworkAddress &address);
	{
		const NetworkAddress na1=sess.address();
		const NetworkAddress netAddr(QHostAddress("127.0.0.1"), 1234);
		QVERIFY(na1!=netAddr);
		sess.setAddress(netAddr);
		const NetworkAddress na2=sess.address();
		QVERIFY(na2==netAddr);
	}



	/////////////////////////////////////////////////
	// void countSend(qint64 written);
	{
		for(int i=0; i<1000; ++i) {
			sess.countSend(123);
		}
	}

	/////////////////////////////////////////////////
	// void receive();
	{
		for(int i=0; i<1000; ++i) {
			sess.receive();
		}
	}

	/////////////////////////////////////////////////
	// bool idle();
	{
		for(int i=0; i<1000; ++i) {
			sess.idle();
		}
	}

	/////////////////////////////////////////////////
	// const QString signatureToString() const;
	{
		QString sigStr=sess.signatureToString();
	}

	/////////////////////////////////////////////////
	// QString summary(QString sep="\n") const ;
	{
		QString sum=sess.summary();
	}

	/////////////////////////////////////////////////
	// QString toString() const ;
	{
		QString str=sess.toString();
	}

	/////////////////////////////////////////////////
	// const QString listText() const;
	{
		QString str=sess.toString();
	}

	/////////////////////////////////////////////////
	// quint64 lastActiveTime() const;
	{
		quint64 lat=sess.lastActiveTime();
	}

	/////////////////////////////////////////////////
	// QVariantMap toVariantMap();
	/////////////////////////////////////////////////
	// void fromVariantMap(const QVariantMap map);
	{
		QVariantMap map1=sess.toVariantMap();

		const qint64 lst=sess.lastSendTime();
		const qint64 lrt=sess.lastRecieveTime();

		QVariantMap wannaMap;
		QVERIFY(map1!=wannaMap);
		wannaMap["lastSendTime"]=lst;
		wannaMap["lastReceiveTime"]=lrt;
		QVERIFY(map1==wannaMap);
		wannaMap["lastSendTime"]=123;
		wannaMap["lastReceiveTime"]=321;
		sess.fromVariantMap(wannaMap);
		QVariantMap map2=sess.toVariantMap();
		if(map2 != wannaMap){
			qDebug()<<"map1="<<map1;
			qDebug()<<"map2="<<map2;
			qDebug()<<"wannaMap="<<wannaMap;
		}
		QCOMPARE(map2, wannaMap);
	}


}



OC_TEST_MAIN(test, TestCommsSession)
