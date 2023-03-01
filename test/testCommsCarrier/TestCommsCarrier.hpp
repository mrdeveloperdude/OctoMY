#ifndef TESTCOMMSCARRIER_HPP
#define TESTCOMMSCARRIER_HPP

#include "test/Common.hpp"

class TestCommsCarrier:public QObject
{
	Q_OBJECT

private slots:
	void testBasic();
	void testConnection();

};


#endif
// TESTCOMMSCARRIER_HPP
