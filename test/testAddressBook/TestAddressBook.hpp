#ifndef TESTADDRESSBOOK_HPP
#define TESTADDRESSBOOK_HPP

#include "test/Common.hpp"

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
	
	/*
	void testFilterEmptyRules();
	void testFilterSingleRule();
	void testFilterMultipleRules();
*/
};

#endif
// TESTADDRESSBOOK_HPP
