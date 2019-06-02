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

public:
	explicit MockCommsCarrierLog(QObject *parent=nullptr);
	virtual ~MockCommsCarrierLog();


public slots:
	void onCarrierReadyRead();
	void onCarrierError(const QString err);
	void onCarrierSendingOpportunity(const quint64 now);
	void onCarrierConnectionStatusChanged(const bool connected);
};


#endif // MOCKCOMMSCARRIERLOG_HPP
