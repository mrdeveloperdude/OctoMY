#ifndef TESTTRIGGER_HPP
#define TESTTRIGGER_HPP

#include "test/Common.hpp"

class TestTrigger:public QObject
{
	Q_OBJECT
	void testCompositeCondition();
	void testManager();
private slots:
	void testVermaster();

};


#endif
// TESTTRIGGER_HPP
