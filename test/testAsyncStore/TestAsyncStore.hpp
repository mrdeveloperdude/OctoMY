#ifndef TESTASYNCSTORE_HPP
#define TESTASYNCSTORE_HPP

#include "../common/TestCommon.hpp"

class TestAsyncStore:public QObject
{
	Q_OBJECT
private slots:
	void testFileExists();
	void testGenerate();
	void testSaveChange();
	void testFails();

};


#endif // TESTASYNCSTORE_HPP
