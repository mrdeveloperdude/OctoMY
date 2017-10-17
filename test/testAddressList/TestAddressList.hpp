#ifndef TESTADDRESSLIST_HPP
#define TESTADDRESSLIST_HPP

#include <QTest>


class TestAddressList:public QObject{
		Q_OBJECT

	private slots:
		void testAddressEntry();
		void testAddressList();
		void testAddressListScore();
		void testAddressListStorage();

};


#endif // TESTADDRESSLIST_HPP
