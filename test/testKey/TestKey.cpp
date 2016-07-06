#include "TestKey.hpp"

#include "security/Key.hpp"


#include <QByteArray>
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
	/*
	QVERIFY(k1.id()==k2.id());
	QVERIFY(k1.id()==k3.id());
	QVERIFY(k1.kid()==k2.kid());
	QVERIFY(k1.kid()==k3.kid());
	QVERIFY(k1.kct()==k2.kct());
	QVERIFY(k1.kct()==k3.kct());
	*/

	qDebug()<<"K1:"<<k3.kid()<< " of " <<k3.kct();
	qDebug()<<"K2:"<<k3.kid()<< " of " <<k3.kct();
	qDebug()<<"K3:"<<k3.kid()<< " of " <<k3.kct();
	Key k4(1024);
	qDebug()<<"K4:"<<k4.kid()<< " of " <<k4.kct()<< " VAR: "<<k4.toVariantMap(false);

	QByteArray orig=QString("THIS IS THE ORIGINAL TEXT").toUtf8();
	QByteArray signature = k1.sign(orig);
	qDebug()<<"ORIG: "<<orig;
	qDebug()<<"SIGN: "<<signature;




	//const auto priPath = QString("/home/lennart/keypairs/private_key.pem"); 	const auto pubPath = QString("/home/lennart/keypairs/public_key.pem"); utility::fileToByteArray(pubPath);
	int ret=0;
	qpolarssl::Pki pkiPrivate;
	qpolarssl::Pki pkiPublic;

	pkiPrivate.generateKeyPair(OCTOMY_KEY_BITS);
	pkiPublic.parsePublicKey(pkiPrivate.getPEMPubkey());

	const QByteArray sourceData = QString("OctoMY™ test data in cleartext").toUtf8();

	const QByteArray signature2 = pkiPrivate.sign(sourceData, OCTOMY_KEY_HASH_POLAR);

	const int siglen=signature2.length();

	if((siglen <= 64) ){
		qWarning()<< "Signature size "<<QString::number(siglen)<<" was less than 64";
	}
	else{
		qDebug()<<"Signature size "<<QString::number(siglen)<<" was OK";
	}
	QVERIFY((siglen > 64) );

	ret = pkiPublic.verify(sourceData, signature2, OCTOMY_KEY_HASH_POLAR);


	if(ret!=0){
		qWarning()<<"Signature verification failed with code -"<<QString::number(-ret,16);
	}
	else{
		qDebug()<<"Signature verification was OK";
	}

	QVERIFY(ret==0);




	QVERIFY(k1.verify(orig, signature));
	QVERIFY(!k4.verify(orig, signature));

}



QTEST_MAIN(TestKey)
