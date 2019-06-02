#ifndef TESTASYNCSTORE_HPP
#define TESTASYNCSTORE_HPP

#include "Common_test.hpp"

class TestAsyncStore:public QObject
{
	Q_OBJECT
private :

private slots:

	void testEvents();
	void testFileExists();
	void testGenerate();
	void testSaveChange();
	void testFails();


	void testConcurrent();
};


#endif
// TESTASYNCSTORE_HPP
