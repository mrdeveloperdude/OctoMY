#ifndef TESTADDRESSLIST_HPP
#define TESTADDRESSLIST_HPP

#include "test/Common.hpp"

class TestAddressList:public QObject
{
	Q_OBJECT

private slots:
	void testAddressEntry();
	void testAddressList();
	void testAddressListScore();
	void testAddressListHighscore();
	void testAddressListStorage();

};


#endif
// TESTADDRESSLIST_HPP
