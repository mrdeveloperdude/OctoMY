#ifndef TESTDISCOVERY_HPP
#define TESTDISCOVERY_HPP

#include <QTest>


class TestDiscovery:public QObject{
		Q_OBJECT

	private slots:
		void test();

};


#endif // TESTDISCOVERY_HPP
