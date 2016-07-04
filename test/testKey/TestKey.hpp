#ifndef TESTKEY_HPP
#define TESTKEY_HPP

#include <QTest>


class TestKey:public QObject{
		Q_OBJECT

	private slots:
		void test();

};


#endif // TESTKEY_HPP
