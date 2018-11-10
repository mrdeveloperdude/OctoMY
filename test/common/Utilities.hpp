#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <QtGlobal>
#include <QString>
#include <QObject>

class QWidget;


class UICloseFilter: public QObject
{
	Q_OBJECT
private:
	QObject &target;
public:
	explicit UICloseFilter(QObject &ob);
	virtual ~UICloseFilter() Q_DECL_OVERRIDE;

public:
	virtual bool eventFilter(QObject *object, QEvent *event) Q_DECL_OVERRIDE ;

signals:

	void close();


};


void testSleep(quint64 ms, QString occation="");

void testHeading(QString msg, QString ch="_");

bool waitForUIEnd(QObject *object, qint64 timeOutMillis=0, const char *customSignal=nullptr);



#endif // UTILITIES_HPP
