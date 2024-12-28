#ifndef TESTQR_HPP
#define TESTQR_HPP

#include "test/Common.hpp"

class TestQr:public QObject
{
	Q_OBJECT
private slots:
	void testBytesToBits();
	void test();

};


#endif
// TESTQR_HPP
