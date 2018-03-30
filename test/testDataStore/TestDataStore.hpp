#ifndef TESTDATASTORE_HPP
#define TESTDATASTORE_HPP

#include "../common/TestCommon.hpp"

class TestDataStore:public QObject
{
	Q_OBJECT

public:
	void testConcurrentQueue();
	void testStressDataStore();

private slots:
	void testDataStore();
	void testDataStoreSyncEmpty();

};


#endif // TESTDATASTORE_HPP
