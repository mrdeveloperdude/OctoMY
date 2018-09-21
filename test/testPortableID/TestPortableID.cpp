#include "TestPortableID.hpp"

#include "security/PortableID.hpp"
#include "utility/Utility.hpp"

#include <QDateTime>

// YOU NEED THIS: http://doc.qt.io/qt-5/qtest.html
void TestPortableID::test(){
	PortableID pid;

	const QString gender="Male";
	QVERIFY(pid.gender() != gender);
	pid.setGender(gender);
	QCOMPARE(pid.gender(), gender);

	const quint64 birthDate=utility::currentMsecsSinceEpoch<quint64>()-1000;
	QVERIFY(pid.birthDate() != birthDate);
	pid.setBirthDate(birthDate);
	QCOMPARE(pid.birthDate(), birthDate);

	const QString id=utility::randomByteArray(20).toHex().toUpper();
	QVERIFY(pid.id() != id);
	pid.setID(id);
	QCOMPARE(pid.id(), id);

	const QString name=utility::randomByteArray(20).toBase64();
	QVERIFY(pid.name() != name);
	pid.setName(name);
	QCOMPARE(pid.name(), name);

	pid.setType(TYPE_UNKNOWN);
	QCOMPARE(pid.type(), TYPE_UNKNOWN);
	pid.setType(TYPE_AGENT);
	QCOMPARE(pid.type(), TYPE_AGENT);
	pid.setType(TYPE_REMOTE);
	QCOMPARE(pid.type(), TYPE_REMOTE);
	pid.setType(TYPE_HUB);
	QCOMPARE(pid.type(), TYPE_HUB);
	pid.setType(TYPE_ZOO);
	QCOMPARE(pid.type(), TYPE_ZOO);

	const QString str=pid.toPortableString();
	qDebug()<<"pid.toPortableString()= "<<str;
	// Should fail
	QVERIFY(!pid.fromPortableString("sdlfjhsdfklhjsdklfjsdklfjsdkljfskldfjklsdjfklsdjf"));
	PortableID pid2;
	const QString str2=pid2.toPortableString();
	qDebug()<<"pid2.toPortableString().1= "<<str2;
	QVERIFY(str != str2);

	QVERIFY(pid.gender() != pid2.gender());
	QVERIFY(pid.birthDate() != pid2.birthDate());
	QVERIFY(pid.id() != pid2.id());
	QVERIFY(pid.name() != pid2.name());
	QVERIFY(pid.type() != pid2.type());

	// Should succeed
	QVERIFY(pid2.fromPortableString(str));

	const QString str3=pid2.toPortableString();
	qDebug()<<"pid2.toPortableString().2= "<<str3;

	QCOMPARE(str, str3);

	QCOMPARE(pid.gender(), pid2.gender());
	QCOMPARE(pid.birthDate(), pid2.birthDate());
	QCOMPARE(pid.id(), pid2.id());
	QCOMPARE(pid.name(), pid2.name());
	QCOMPARE(pid.type(), pid2.type());


}



OC_TEST_MAIN(test, TestPortableID)
