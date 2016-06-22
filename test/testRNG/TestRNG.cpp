#include "TestRNG.hpp"

#include "random/RNG.hpp"

// YOU NEED THIS: http://doc.qt.io/qt-5/qtest.html
void TestRNG::test(){
	const int ct=20;
	const QByteArray init1=QByteArray("\x00\x00\x00\x03\x00\x00\x00\x04\x00\x00\x00\x05",12);
	const QByteArray init2=QString("This right here is some motherfracking style").toUtf8();
	QStringList brands;
	brands<<"mt"<<"pm"<<"fast";
	for(QString brand:brands){
		qDebug()<<"";
		qDebug()<<"############# "<<brand <<" ##############";
		RNG * const rng1=RNG::sourceFactory(brand);
		RNG * const rng2=RNG::sourceFactory(brand);
		RNG * const rng3=RNG::sourceFactory(brand);
		RNG * const rng4=RNG::sourceFactory(brand);
		QVERIFY(nullptr!=rng1);
		QVERIFY(nullptr!=rng2);
		QVERIFY(nullptr!=rng3);
		QVERIFY(nullptr!=rng4);
		qDebug()<<"Initializing rng1: "<<rng1->getName()<<"-----------";
		rng1->init(init1);
		qDebug()<<"Initializing rng2: "<<rng2->getName()<<"-----------";
		rng2->init(init1);
		qDebug()<<"Initializing rng3: "<<rng3->getName()<<"-----------";
		rng3->init(init2);
		qDebug()<<"Initializing rng4: "<<rng4->getName()<<"-----------";
		rng4->init(init1);
		qDebug()<<"Testing 1 vs 2 vs 3";
		for(int i=0;i<ct;++i){
			const quint32 v1=rng1->generateInt32();
			const quint32 v2=rng2->generateInt32();
			const quint32 v3=rng3->generateInt32();
			qDebug()<<"RUN="<<i<<": V1="<<v1<<", V2="<<v2<<", V3="<<v3;
			QVERIFY(v1==v2);
			QVERIFY(v1!=v3);
		}
		qDebug()<<"Re-initializing rng1: "<<rng1->getName()<<"-----------";
		rng1->init(init1);
		qDebug()<<"Re-initializing rng2: "<<rng2->getName()<<"-----------";
		rng2->init(init1);
		qDebug()<<"Re-initializing rng3: "<<rng3->getName()<<"-----------";
		rng3->init(init1);
		qDebug()<<"Testing 1 vs 2 vs 3 vs 4";
		for(int i=0;i<ct;++i){
			const quint32 v1=rng1->generateInt32();
			const quint32 v2=rng2->generateInt32();
			const quint32 v3=rng3->generateInt32();
			const quint32 v4=rng4->generateInt32();
			qDebug()<<"RUN="<<i<<": V1="<<v1<<", V2="<<v2<<", V3="<<v3<<", V4="<<v4;
			QVERIFY2(v1==v2, QString(QStringLiteral("For i =")+QString::number(i)).toStdString().c_str());
			QVERIFY2(v1==v3, QString(QStringLiteral("For i =")+QString::number(i)).toStdString().c_str());
			QVERIFY2(v1==v4, QString(QStringLiteral("For i =")+QString::number(i)).toStdString().c_str());
		}
		delete rng1;
		delete rng2;
		delete rng3;
		delete rng4;
	}
}



QTEST_MAIN(TestRNG)
