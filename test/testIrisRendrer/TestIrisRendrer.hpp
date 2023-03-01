#ifndef TESTIRISRENDRER_HPP
#define TESTIRISRENDRER_HPP

#include "test/Common.hpp"


class TestIrisRendrer:public QObject
{
	Q_OBJECT
private:
	void testUI();
	void testSave();

private slots:
	void testHuge();

};


#endif
// TESTIRISRENDRER_HPP
