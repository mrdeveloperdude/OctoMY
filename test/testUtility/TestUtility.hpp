#ifndef TESTUTILITY_HPP
#define TESTUTILITY_HPP

#include "../common/TestCommon.hpp"


class TestUtility:public QObject
{
	Q_OBJECT

private slots:

	void testIPv4();
	void testIPv6();

};


#endif // TESTUTILITY_HPP
