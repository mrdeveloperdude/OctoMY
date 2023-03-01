#ifndef COMMSSIGNALLOGGER_HPP
#define COMMSSIGNALLOGGER_HPP

#include <QString>
#include <QObject>
#include "test/Common.hpp"


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
	void onCommsConnectionStatusChanged(const bool isConnected, const bool needsConnection);

};


#endif
// COMMSSIGNALLOGGER_HPP
