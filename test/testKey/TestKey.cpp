#include "TestKey.hpp"

#include "security/Key.hpp"

// YOU NEED THIS: http://doc.qt.io/qt-5/qtest.html
void TestKey::test(){

	Key k1(1024);
	qDebug()<<"K1:"<<k1.id();
	Key k2(k1);
	qDebug()<<"K2:"<<k2.id();
	qDebug()<<"K1:"<<k1.id();
	Key k3(1024);
	qDebug()<<"K3:"<<k3.id();
	k3=k1;
	qDebug()<<"K3:"<<k3.id();
	QVERIFY(k1.id()==k2.id());
	QVERIFY(k1.id()==k3.id());
	QVERIFY(k1.kid()==k2.kid());
	QVERIFY(k1.kid()==k3.kid());
	QVERIFY(k1.kct()==k2.kct());
	QVERIFY(k1.kct()==k3.kct());

	qDebug()<<"K1:"<<k3.kid()<< " of " <<k3.kct();
	qDebug()<<"K2:"<<k3.kid()<< " of " <<k3.kct();
	qDebug()<<"K3:"<<k3.kid()<< " of " <<k3.kct();

}



QTEST_MAIN(TestKey)
