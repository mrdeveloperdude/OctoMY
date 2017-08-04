#ifndef COMMSSIGNALLOGGER_HPP
#define COMMSSIGNALLOGGER_HPP


#include  <QString>
#include <QObject>
#include <QTest>


class CommsSession;

class CommsSignalLogger: public QObject
{
	Q_OBJECT

private:
	QString mName;
public:
	explicit CommsSignalLogger(QString name, QObject *parent=nullptr);
	virtual ~CommsSignalLogger();

public slots:
	void onCommsError(QString message);
	void onCommsClientAdded(CommsSession *c);
	void onCommsConnectionStatusChanged(bool c);

};


#endif // COMMSSIGNALLOGGER_HPP
