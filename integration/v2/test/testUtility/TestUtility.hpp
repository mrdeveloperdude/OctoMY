#ifndef TESTUTILITY_HPP
#define TESTUTILITY_HPP

#include "Common_test.hpp"


class TestUtility:public QObject
{
	Q_OBJECT

private slots:
	// From testUtilityNetwork.inc.cpp
	void testIPv4();
	void testIPv6();
	// From testUtilityRandom.inc.cpp
	void testRandom();
	// From testUtilityTime.inc.cpp
	void testTime();
	// From testUtilityString.inc.cpp
	void testString();

};


#endif
// TESTUTILITY_HPP
