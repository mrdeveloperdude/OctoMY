#ifndef TESTADDRESSBOOK_HPP
#define TESTADDRESSBOOK_HPP

#include "Common_test.hpp"

class TestAddressBook:public QObject
{
	Q_OBJECT

private slots:
	void testToFromGenerateMap();
	void testAss();
	void testPins();
	void testTrusts();
	void testOperators();
	void testCreate();
	void testLoad();

};

#endif
// TESTADDRESSBOOK_HPP
