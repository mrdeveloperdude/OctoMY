#ifndef TESTLOGHANDLER_HPP
#define TESTLOGHANDLER_HPP

#include "test/Common.hpp"


class TestLogHandler:public QObject
{
	Q_OBJECT
private slots:
	void test();

};


#endif
// TESTLOGHANDLER_HPP
