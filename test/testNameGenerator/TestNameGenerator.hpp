#ifndef TESTNAMEGENERATOR_HPP
#define TESTNAMEGENERATOR_HPP

#include "test/Common.hpp"

class TestNameGenerator:public QObject
{
	Q_OBJECT

private slots:
	void testAgent();
	void testRemote();

};


#endif
// TESTNAMEGENERATOR_HPP
