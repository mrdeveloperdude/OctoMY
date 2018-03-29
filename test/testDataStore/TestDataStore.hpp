#ifndef TESTDATASTORE_HPP
#define TESTDATASTORE_HPP

#include "../common/TestCommon.hpp"

class TestDataStore:public QObject
{
	Q_OBJECT
private slots:

	void testConcurrentQueue();
	void testDataStore();
	void testStressDataStore();
public:
};


#endif // TESTDATASTORE_HPP
