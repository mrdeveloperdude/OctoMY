#ifndef STRESS_HPP
#define STRESS_HPP

#include <QObject>
#include <QDateTime>
#include <QThread>
#include <QThreadPool>



class Stress : public QObject
{
	Q_OBJECT
private:

	QThreadPool tp;
	int cores;

public:
	explicit Stress();
	void setCPUPct(qint64 pct);
	void setMEMPct(int pct);
	void setActive(bool active);
private:
	void burnOnAllCores();

};

#endif // STRESS_HPP
