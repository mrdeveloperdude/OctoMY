#ifndef TESTNAMEGENERATOR_HPP
#define TESTNAMEGENERATOR_HPP

#include "../common/TestCommon.hpp"

class TestNameGenerator:public QObject
{
	Q_OBJECT
private slots:

	void testAgent();

	void testRemote();


};


#endif // TESTNAMEGENERATOR_HPP
