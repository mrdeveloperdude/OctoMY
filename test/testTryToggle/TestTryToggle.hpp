#ifndef TESTTRYTOGGLE_HPP
#define TESTTRYTOGGLE_HPP

#include <QTest>


class TestTryToggle:public QObject
{

	Q_OBJECT

	void test();

private slots:


	void testUI();

};


#endif // TESTTRYTOGGLE_HPP
