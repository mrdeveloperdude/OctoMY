#ifndef TESTTEMPLATE_HPP
#define TESTTEMPLATE_HPP

#include <QTest>


class TestTemplate:public QObject{
		Q_OBJECT

	private slots:
		void test();

};


#endif // TESTTEMPLATE_HPP
