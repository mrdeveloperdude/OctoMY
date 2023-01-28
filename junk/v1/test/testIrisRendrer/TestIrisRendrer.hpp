#ifndef TESTIRISRENDRER_HPP
#define TESTIRISRENDRER_HPP

#include "../common/TestCommon.hpp"


class TestIrisRendrer:public QObject
{
	Q_OBJECT

	void testUI();
	void testSave();

private slots:


	void testHuge();



};


#endif // TESTIRISRENDRER_HPP
