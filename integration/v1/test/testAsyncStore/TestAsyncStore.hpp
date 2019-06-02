#ifndef TESTASYNCSTORE_HPP
#define TESTASYNCSTORE_HPP

#include "../common/TestCommon.hpp"

class TestAsyncStore:public QObject
{
	Q_OBJECT
private :
	void testEvents();

	void testFileExists();
	void testGenerate();
	void testSaveChange();
	void testFails();


private slots:

	void testConcurrent();
};


#endif // TESTASYNCSTORE_HPP
