#ifndef TESTFORK_HPP
#define TESTFORK_HPP

#include <QObject>

class TestFork:public QObject
{
	Q_OBJECT
public slots:
	void test();

};


#endif
// TESTFORK_HPP
