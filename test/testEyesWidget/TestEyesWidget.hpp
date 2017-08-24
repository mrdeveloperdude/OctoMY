#ifndef TESTEYESWIDGET_HPP
#define TESTEYESWIDGET_HPP

#include <QTest>


class TestEyesWidget:public QObject{
		Q_OBJECT


	private slots:
		void testWidget();
		void testRendrer();


};


#endif // TESTEYESWIDGET_HPP
