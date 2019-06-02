#ifndef TESTCONCURRENTQUEUE_HPP
#define TESTCONCURRENTQUEUE_HPP

#include "Common_test.hpp"



class TestConcurrentQueue:public QObject
{
	Q_OBJECT

private slots:
	void testActivation();
	void testExecution();

};


#endif
// TESTCONCURRENTQUEUE_HPP
