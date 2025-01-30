#ifndef TESTCOMMSCARRIER_HPP
#define TESTCOMMSCARRIER_HPP

#include "test/Common.hpp"

class TestCommsCarrier:public QObject
{
	Q_OBJECT
private:
	void testBasic();
	void testConnection();
	
private slots:
	void testInteractive();

};


#endif
// TESTCOMMSCARRIER_HPP
