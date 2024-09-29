#ifndef UTILITY_TEST_HPP
#define UTILITY_TEST_HPP

#include <QString>
#include <QObject>

class QWidget;

namespace test
{

namespace utility
{


class UICloseFilter: public QObject
{
	Q_OBJECT
private:
	QObject &target;
public:
	explicit UICloseFilter(QObject &ob);
	virtual ~UICloseFilter() override;

public:
	virtual bool eventFilter(QObject *object, QEvent *event) override ;

signals:
	void close();

};


void testSleep(quint64 ms, QString occation="");
void testWaitForEvents();
void testHeading(QString msg, QString ch="_");
bool waitForUIEnd(QObject *object, qint64 timeOutMillis=0, const char *customSignal=nullptr);

}
}

#endif
// UTILITY_TEST_HPP
