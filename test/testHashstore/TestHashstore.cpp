#include "TestHashstore.hpp"

#include "storage/Hashstore.hpp"

// YOU NEED THIS: http://doc.qt.io/qt-5/qtest.html

void TestHashstore::test(){
	QDir dir("./");
	QVERIFY(dir.exists());
	Hashstore store(dir);
	QString key1="LOLBOB";
	QString key2="LOLFOOBAR";
	QString key3="ABRAKADABRA";
	HashstoreRecord rec0=store.resolve(key1);
	HashstoreRecord rec1=store.resolve(key1);
	HashstoreRecord rec2=store.resolve(key2);
	HashstoreRecord rec3=store.resolve(key3);
	QVERIFY(rec0==rec1);
	QVERIFY(rec0!=rec2);
	QVERIFY(rec1!=rec2);
	QVERIFY(rec1!=rec3);
	QVERIFY(rec2!=rec3);
	QByteArray ba_truth;
	QByteArray ba;
	qDebug()<<"BAT ORIG: "<<QString::fromLatin1(ba_truth);
	qDebug()<<"BA ORIG: "<<QString::fromLatin1(ba);
	rec0.put(ba);
	for(int i=0;i<100;++i){
		ba=rec0.get();
		QByteArray m="LALALA_"+QString::number(i).toUtf8();
		qDebug()<<"BAT BEFORE: "<<QString::fromLatin1(ba_truth);
		qDebug()<<"BA BEFORE: "<<QString::fromLatin1(ba);
		ba+=m;

		ba_truth+=m;
		rec0.put(ba);
		QString trut="["+QString::fromLatin1(ba)+QStringLiteral("] vs. [")+QString::fromLatin1(ba_truth)+"]";
		QVERIFY2(ba==ba_truth,trut.toStdString().c_str());
	}


}



QTEST_MAIN(TestHashstore)
