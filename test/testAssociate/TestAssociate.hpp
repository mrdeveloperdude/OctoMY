#ifndef TESTASSOCIATE_HPP
#define TESTASSOCIATE_HPP

#include "Common_test.hpp"

class TestAssociate:public QObject
{
	Q_OBJECT

private slots:
	void test();
	void testMapConversions();
	void testTimeConversions();

};

#endif
// TESTASSOCIATE_HPP
