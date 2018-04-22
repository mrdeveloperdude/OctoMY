#ifndef TESTCOMMSCARRIER_HPP
#define TESTCOMMSCARRIER_HPP

#include "../common/TestCommon.hpp"

class TestCommsCarrier:public QObject
{
	Q_OBJECT
private slots:

	void testBasic();

	void testConnection();

};


#endif // TESTCOMMSCARRIER_HPP
