#ifndef STRESSQR_HPP
#define STRESSQR_HPP

#include <QTest>


class StressQR:public QObject
{
	Q_OBJECT

private slots:
	void stress();

};


#endif // STRESSQR_HPP
