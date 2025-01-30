#ifndef MOCKCOMMSCARRIERLOG_HPP
#define MOCKCOMMSCARRIERLOG_HPP

#include <QString>
#include <QObject>

class MockCommsCarrierLog: public QObject
{
	Q_OBJECT
private:
	quint64 mLastPrint;
	quint64 mOpportunityCount;
	QString mSender;

public:
	explicit MockCommsCarrierLog(QObject *parent=nullptr, const QString &sender = QString("Unknown"));
	virtual ~MockCommsCarrierLog();

public slots:
	void onCarrierReadyRead();
	void onCarrierError(const QString err);
	void onCarrierSendingOpportunity(const quint64 now);
	void onCarrierConnectionStatusChanged(const bool connected);
	
signals:
	void logMessage(const QString &sender, const QString &message, const QString &type);

};


#endif
// MOCKCOMMSCARRIERLOG_HPP
