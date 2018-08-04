#include "TestFivePhase.hpp"

#include "ardumy/FivePhase.hpp"

#include <QtGlobal>

template<typename T>
static QString intToStr(T value)
{
	return QString("%1").arg(value, sizeof(T)*8, 2, QLatin1Char('0'))
		   + QString(" ( 0x%1").arg(value,  (sizeof(T)*8)/4, 16, QLatin1Char('0'))
		   + QString(", %1 )").arg(value);

}


#include <iostream>

template<typename T>
void printTable(T *table, size_t size)
{
	qDebug() << "TABLE("<<size<<"){";
	for(size_t i=0; i<size; ++i) {
		qDebug().nospace().noquote()<< "\t"<<intToStr(table[i]);
	}
	qDebug() << "}";
}





void TestFivePhase::test()
{
	FivePhase fp;
	size_t stepCount=10;
	uint16_t *single_table=new uint16_t[stepCount];
	for(size_t i=0; i<stepCount; ++i) {
		single_table[i]=0;
	}
	const size_t phaseCount=5;
	const uint16_t stateCount=4;
	const uint16_t phaseShift=0;
	const uint16_t startState=0;
	fp.bob(single_table, stepCount, phaseCount, phaseShift, (uint16_t)0, stateCount, startState);
	fp.bob(single_table, stepCount, phaseCount, phaseShift, (uint16_t)7, stateCount, (uint16_t)(startState+1));
	printTable(single_table, 10);

}


OC_TEST_MAIN(test, TestFivePhase)

