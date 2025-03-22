#ifndef TESTSCREEN_HPP
#define TESTSCREEN_HPP

#include "test/Common.hpp"

class TestScreen:public QObject
{
	Q_OBJECT
private slots:
	void testManager();
	void testSelector();

};


#endif
// TESTSCREEN_HPP
