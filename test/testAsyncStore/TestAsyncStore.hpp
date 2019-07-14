#ifndef TESTASYNCSTORE_HPP
#define TESTASYNCSTORE_HPP

#include "Common_test.hpp"

class TestAsyncStore:public QObject
{
	Q_OBJECT
private :

private slots:
	// Test complete integration running
	void testConcurrent();
	// Test that all events work
	void testEvents();
	// Set up test where no file exist and no data is loaded to trigger generation mechanism in synchronize
	void testGenerationBySync();
	// Set up test where a file exist but no data is loaded to trigger loading mechnism in synchronize
	void testLoadingBySync();
	// Set up test where a file exists and data is changed to trigger saving mechanism in synchronize
	void testSavingBySync();
	// Set up test to fail in different ways
	void testFailures();

};


#endif
// TESTASYNCSTORE_HPP
