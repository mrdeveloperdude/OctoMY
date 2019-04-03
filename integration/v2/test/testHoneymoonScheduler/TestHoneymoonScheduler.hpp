#ifndef TESTHONEYMOONSCHEDULER_HPP
#define TESTHONEYMOONSCHEDULER_HPP

#include "Common_test.hpp"

class TestHoneymoonScheduler:public QObject
{
	Q_OBJECT
private:
	void generateTestCase(const int minV, const int maxV , const quint64 graceP , const quint64 decayP , const quint64 interval );

private slots:
	void testActivation();
	void testNormalUse();
	void testaAbuse();

};


#endif
// TESTHONEYMOONSCHEDULER_HPP
