#ifndef TESTSIMPLEDATASTORE_HPP
#define TESTSIMPLEDATASTORE_HPP

#include "test/Common.hpp"

class TestSimpleDataStore:public QObject
{
	Q_OBJECT

private slots:
	void testSynchronous();
	void testAsynchronous();

};


#endif
// TESTSIMPLEDATASTORE_HPP
