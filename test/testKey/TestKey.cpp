#include "TestKey.hpp"

#include "security/Key.hpp"
#include "security/SecurityConstants.hpp"
#include "security/SecurityConstantsPrivate.hpp"

#include "qpolarssl/qpolarsslpki.hpp"

#include "uptime/New.hpp"
#include "utility/random/Random.hpp"


#include <QByteArray>


class ContainerClass
{
private:
	//CommsSignature mSignature;
	Key &mKey;

	public:

		//Client(QHostAddress host, quint16 port, LogDestination *log=0);
		explicit ContainerClass(Key &key);
		virtual ~ContainerClass();
};


ContainerClass::ContainerClass(Key &key)
	: mKey(key)
{
	if(nullptr == &key){
		qWarning()<<"ERROR: key was nullptr";
	}
}


ContainerClass::~ContainerClass()
{

}


void TestKey::testAllocation()
{
	QSharedPointer<ContainerClass> cc(nullptr);
	Key key;
	cc=QSharedPointer<ContainerClass>(OC_NEW ContainerClass(key));
}


void TestKey::testBasicIntegrity()
{
	// ############################# #############################
	// ############################# explicit Key(QVariantMap map, bool isPublic);


	// ############################# #############################
	// ############################# explicit Key(KeyPrivate &dd);

	// ############################# #############################
	// ############################# Key(const Key & other);

	// ############################# #############################
	// ############################# Key(Key && other);


	// ############################# #############################
	// ############################# bool operator==(Key &other);


	// ############################# #############################
	// ############################# friend void swap(Key& first, Key& second) /* nothrow */;

	// ############################# #############################
	// ############################# static QString hash(QString input);





	// ############################# #############################
	// ############################# QString toString();

	// ############################# #############################
	// ############################# bool isValid(bool onlyPublic);


	// ############################# #############################
	// ############################# explicit Key(quint32 bits);
	Key k1(1024);

	QCOMPARE(k1.kid(), 1);
	QCOMPARE(k1.kct(), 2);


	// ############################# #############################
	// ############################# QString id() const;
	qDebug()<<"K1:"<<k1.id();

	// ############################# #############################
	// ############################# explicit Key(QString key, bool isPublic);
	Key k2(k1);

	QCOMPARE(k1.kid(), 1);
	QCOMPARE(k1.kct(), 3);
	QCOMPARE(k2.kid(), 2);
	QCOMPARE(k2.kct(), 3);


	qDebug()<<"K2:"<<k2.id();
	qDebug()<<"K1:"<<k1.id();
	Key k3(1024);
	qDebug()<<"K3:"<<k3.id();

	// ############################# #############################
	// ############################# bool operator!=(Key &other);

	QVERIFY(k1!=k3);
	QVERIFY(!(k1==k3));
	QVERIFY(k1.id()!=k3.id());


	// ############################# #############################
	// ############################# Key & operator=(Key other);
	k3=k1;



	qDebug()<<"K3:"<<k3.id();

	// ############################# #############################
	// ############################# bool operator==(Key &other);
	QVERIFY(k1==k2);
	QVERIFY(!(k1!=k2));
	QVERIFY(k1.id()==k2.id());


	QVERIFY(k1==k3);
	QVERIFY(k1.id()==k3.id());

	// ############################# #############################
	// ############################# int kid();

	// ############################# #############################
	// ############################# int kct();

	qDebug()<<"K1:"<<k3.kid()<< " of " <<k3.kct();
	qDebug()<<"K2:"<<k3.kid()<< " of " <<k3.kct();
	qDebug()<<"K3:"<<k3.kid()<< " of " <<k3.kct();

	QVERIFY(k1.id()==k2.id());
	QVERIFY(k1.id()==k3.id());
	QVERIFY(k1.kid()!=k2.kid());
	QVERIFY(k1.kid()!=k3.kid());


	// ############################# #############################
	// ############################# QString key();

	// ############################# #############################
	// ############################# QString pubKey();

	// ############################# #############################
	// ############################# bool hasPublic(bool thatIsValid=false);

	// ############################# #############################
	// ############################# bool hasPrivate(bool thatIsValid=false);

	// ############################# #############################
	// ############################# QVariantMap toVariantMap(bool onlyPublic);


	QVariantMap k1PubMap=k1.toVariantMap(true);

	QVERIFY(k1PubMap.contains("publicKey"));
	QString k1PubMapPuk=k1PubMap["publicKey"].toString();
	QVERIFY(!k1PubMapPuk.isEmpty());
	QCOMPARE(k1PubMapPuk, k1.pubKey());

	QVERIFY(k1PubMap.contains("id"));
	QString k1PubMapID=k1PubMap["id"].toString();
	QVERIFY(!k1PubMapID.isEmpty());
	QCOMPARE(k1PubMapID, k1.id());

	QVERIFY(!k1PubMap.contains("privateKey"));

	QCOMPARE(k1PubMap.size(), 2);


	QVariantMap k1PrivMap=k1.toVariantMap(false);

	QVERIFY(k1PrivMap.contains("publicKey"));
	QString k1PrivMapPuk=k1PrivMap["publicKey"].toString();
	QVERIFY(!k1PrivMapPuk.isEmpty());
	QCOMPARE(k1PrivMapPuk, k1.pubKey());

	QVERIFY(k1PrivMap.contains("id"));
	QString k1PrivMapID=k1PrivMap["id"].toString();
	QVERIFY(!k1PrivMapID.isEmpty());
	QCOMPARE(k1PrivMapID, k1.id());

	QVERIFY(k1PrivMap.contains("privateKey"));
	QString k1PrivMapPrk=k1PrivMap["privateKey"].toString();
	QVERIFY(!k1PrivMapPrk.isEmpty());
	QCOMPARE(k1PrivMapPrk, k1.key());

	QCOMPARE(k1PrivMap.size(), 3);

	// ############################# #############################
	// ############################# explicit Key(QVariantMap map, bool isPublic);


	Key k4a(k1PubMap,true);
	qDebug()<<"K4-A:"<<k4a.id();
	QVERIFY(k4a.isValid(true));
	QVERIFY(""!=k4a.id());
	QVERIFY(""!=k4a.pubKey());
	QVERIFY(""==k4a.key());
	QVERIFY(k4a.hasPublic());
	QVERIFY(k4a.hasPublic(false));
	QVERIFY(k4a.hasPublic(true));
	QVERIFY(!k4a.hasPrivate());
	QVERIFY(!k4a.hasPrivate(false));
	QVERIFY(!k4a.hasPrivate(true));

	Key k5a(k1PrivMap,false);
	qDebug()<<"K5-A:"<<k5a.id();
	QVERIFY(k5a.isValid(true));
	QVERIFY(""!=k5a.id());
	QVERIFY(""!=k5a.pubKey());
	QVERIFY(""!=k5a.key());
	QVERIFY(k5a.hasPublic());
	QVERIFY(k5a.hasPublic(false));
	QVERIFY(k5a.hasPublic(true));
	QVERIFY(k5a.hasPrivate());
	QVERIFY(k5a.hasPrivate(false));
	QVERIFY(k5a.hasPrivate(true));

	Key k4b(k1PubMap,false);
	qDebug()<<"K4-B:"<<k4b.id();
	QVERIFY(k4b.id().isEmpty());
	QVERIFY(!k4b.isValid(true));
	QVERIFY(!k4b.isValid(false));
	QVERIFY(!k4b.hasPublic());
	QVERIFY(!k4b.hasPublic(false));
	QVERIFY(!k4b.hasPublic(true));
	QVERIFY(!k4b.hasPrivate());
	QVERIFY(!k4b.hasPrivate(false));
	QVERIFY(!k4b.hasPrivate(true));

	Key k5b(k1PrivMap,true);
	qDebug()<<"K5-B:"<<k5b.id();
	QVERIFY(k5b.isValid(true));
	QVERIFY(!k5b.isValid(false));
	QVERIFY(k5b.hasPublic());
	QVERIFY(k5b.hasPublic(false));
	QVERIFY(k5b.hasPublic(true));
	QVERIFY(!k5b.hasPrivate());
	QVERIFY(!k5b.hasPrivate(false));
	QVERIFY(!k5b.hasPrivate(true));

	QVariantMap emptyMap;

	Key k6a(emptyMap,true);
	qDebug()<<"K6-A:"<<k6a.id();
	QVERIFY(k6a.id().isEmpty());
	QVERIFY(!k6a.isValid(true));
	QVERIFY(!k6a.isValid(false));
	QVERIFY(!k6a.hasPublic());
	QVERIFY(!k6a.hasPublic(false));
	QVERIFY(!k6a.hasPublic(true));
	QVERIFY(!k6a.hasPrivate());
	QVERIFY(!k6a.hasPrivate(false));
	QVERIFY(!k6a.hasPrivate(true));

	Key k6b(emptyMap,false);
	qDebug()<<"K6-B:"<<k6b.id();
	QVERIFY(k6b.id().isEmpty());
	QVERIFY(!k6b.isValid(true));
	QVERIFY(!k6b.isValid(false));
	QVERIFY(!k6b.hasPublic());
	QVERIFY(!k6b.hasPublic(false));
	QVERIFY(!k6b.hasPublic(true));
	QVERIFY(!k6b.hasPrivate());
	QVERIFY(!k6b.hasPrivate(false));
	QVERIFY(!k6b.hasPrivate(true));
}


void TestKey::testSignature()
{
	Key k1(1024);
	Key k6(1024);
	qDebug()<<"K6:"<<k6.kid()<< " of " <<k6.kct()<< " VAR: "<<k6.toVariantMap(false);

	// ############################# #############################
	// ############################# QByteArray sign(const QByteArray &source);


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

	if((siglen <= 64) ) {
		qWarning()<< "Signature size "<<QString::number(siglen)<<" was less than 64";
	} else {
		qDebug()<<"Signature size "<<QString::number(siglen)<<" was OK";
	}
	QVERIFY((siglen > 64) );

	// ############################# #############################
	// ############################# bool verify(const QByteArray &message, const QByteArray &signature);


	ret = pkiPublic.verify(sourceData, signature2, OCTOMY_KEY_HASH_POLAR);


	if(ret!=0) {
		qWarning()<<"Signature verification failed with code -"<<QString::number(-ret,16);
	} else {
		qDebug()<<"Signature verification was OK";
	}

	QVERIFY(ret==0);
	QVERIFY(k1.verify(orig, signature));
	QVERIFY(!k6.verify(orig, signature));
}


void TestKey::testEncryption()
{
	// ############################# #############################
	// ############################# QByteArray encrypt(const QByteArray& data);

	// ############################# #############################
	// ############################# QByteArray decrypt(const QByteArray& data);

	//for(int j=7; j<13; ++j)
	const int j=8;
	{
		const int bitsize=1<<j;
		Key k1(bitsize);
		QVERIFY(k1.isValid(true));
		QVERIFY(k1.isValid(false));
		qDebug()<<"";
		qDebug()<<"__________________________________";
		qDebug()<<"---- K1 DESCRIPTION: -------------";
		k1.describe();
		/*
				k1.
				NONE,
				RSA,
				ECKEY,
				ECKEY_DH,
				ECDSA,
				RSA_ALT,
				RSASSA_PSS
		*/
		// Separate the pubkey into it's own instance
		Key k2(k1.pubKey(), true);
		QVERIFY(k2.isValid(true));
		QVERIFY(!k2.isValid(false));
		qDebug()<<"";
		qDebug()<<"__________________________________";
		qDebug()<<"---- K2 DESCRIPTION: -------------";
		k2.describe();
		//for(int i=0; i<16; ++i)
		for(int i=0; i<30; ++i) {
			//const int datasize=1<<i;
			const int datasize=i+1;

			{
				qDebug()<<"";
				qDebug()<<"";
				qDebug()<<"####################################################################";
				qDebug()<<"----- BITSIZE: "<<bitsize<<", DATASIZE: "<<datasize;
				QByteArray orig1=utility::random::randomByteArray(datasize);

				const QByteArray encrypted1= k1.encrypt(orig1);
				const QByteArray decrypted1= k1.decrypt(encrypted1);

				qDebug()<<"ORIG1: "<<orig1;
				qDebug()<<"ENCR1: "<<encrypted1;
				qDebug()<<"DECR1: "<<decrypted1;

				//QCOMPARE(orig1,decrypted1);
			}
			{
				QByteArray orig2=utility::random::randomByteArray(datasize);
				const QByteArray encrypted2 = k2.encrypt(orig2);
				const QByteArray decrypted2= k1.decrypt(encrypted2);

				qDebug()<<"ORIG2: "<<orig2;
				qDebug()<<"ENCR2: "<<encrypted2;
				qDebug()<<"DECR2: "<<decrypted2;

				//QCOMPARE(orig2,decrypted2);
			}
		}
	}
}


OC_TEST_MAIN(test, TestKey)
