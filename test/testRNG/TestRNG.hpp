#ifndef TESTRNG_HPP
#define TESTRNG_HPP

#include <QTest>


class TestRNG:public QObject{
		Q_OBJECT

	private slots:
		void test();

};


#endif // TESTRNG_HPP
