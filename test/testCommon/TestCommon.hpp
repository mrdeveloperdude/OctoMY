#ifndef TESTCOMMON_HPP
#define TESTCOMMON_HPP

#include <QTest>


class TestCommon:public QObject{
		Q_OBJECT

	private slots:
		void test();

};


#endif // TESTCOMMON_HPP
