#include "TestIDDuel.hpp"

#include "comms/IDDuel.hpp"
#include "utility/random/Random.hpp"

void TestIDDuel::test(){
	for(int i=0;i<1000;++i){
		const auto a=qrand();
		const auto b=qrand();
		const auto strA=QString("%1").arg(a, 12, 10, QChar('0'));
		const auto strB=QString("%1").arg(b, 12, 10, QChar('0'));
		const bool expectAGreaterThanB=(a>b);
		qDebug()<<"A: "<<a<<", str-A: "<<strA<<" "<<( expectAGreaterThanB?">":"<") << "  B: "<<b<< ", str-B: "<<strB;
		IDDuel duelBasic(strA,strB);
		QCOMPARE(expectAGreaterThanB, duelBasic.duel());

		const QString prefix=utility::random::randomByteArray(10).toBase64();
		const QString prefixA=prefix+strA;
		const QString prefixB=prefix+strB;
		qDebug()<<"PREFIX: "<<prefix<<", PREFIX-A: "<<prefixA<<", PREFIX-A: "<<prefixB;
		IDDuel duelPrefix(prefixA, prefixB);
		QCOMPARE(expectAGreaterThanB, duelPrefix.duel());

		const QString postfix=utility::random::randomByteArray(10).toBase64();
		const QString postfixA=strA+postfix;
		const QString postfixB=strB+postfix;
		qDebug()<<"POSTFIX: "<<postfix<<", POSTFIX-A: "<<postfixA<<", POSTFIX-A: "<<postfixB;
		IDDuel duelPostfix(postfixA, postfixB);
		QCOMPARE(expectAGreaterThanB, duelPostfix.duel());

	}
}


OC_TEST_MAIN(test, TestIDDuel)
