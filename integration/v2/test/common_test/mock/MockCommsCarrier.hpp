#ifndef MOCKCOMMSCARRIER_HPP
#define MOCKCOMMSCARRIER_HPP

#include "comms/CommsCarrier.hpp"

#include <QObject>
#include <QUdpSocket>
#include <QString>
#include <QByteArray>
#include <QHash>
#include <QPair>

class MockCommsCarrier: public CommsCarrier
{
	Q_OBJECT
private:
	QHash<QString, QList<QByteArray> > mMockWriteData;
	QList< QPair<QString, QByteArray> > mMockReadData;
	bool mHasPendingData;
	qint64 mWriteBatchSize;
	bool mIsStarted;
	bool mStartFail;
	QString mLastError;
	quint64 mMinimalPacketInterval;
	quint64	mMaximalPacketInterval;
	NetworkAddress mOurAddress;
	bool mOverrideStartStop;

public:
	explicit MockCommsCarrier(QObject *parent=nullptr);
	virtual ~MockCommsCarrier();

	////////////////////////// Mock interface
public:
	void mockWriteMock(QByteArray data, const NetworkAddress &address, bool sendReadyReadSignal=true);
	QByteArray mockReadMock(const NetworkAddress &address);
	void mockSetHasPendingData(bool hasPendingData);
	void mockSetWriteBatchSize(qint64 writeBatchSize=-1);
	void mockSetStartFail(bool startFail=true);
	void mockSetMinimalPacketInterval(quint64 size);
	void mockSetMaximalPacketIntervalImp(quint64 size);
	void mockSetAddress(NetworkAddress addr);
	void mockSetOverrideSendingtimer(bool override);

	void mockStartSendingTimer();
	void mockStopSendingTimer();

	void mockTriggerReadyRead();
	void mockTriggerErrorSignal(QString error);
	void mockTriggerSendingOpportunity(quint64 now);
	void mockTriggerConnectionStatusChanged(bool connected);


	// CommsCarrier overrides to take countrol over sending opportunity timer
public slots:
	bool start(NetworkAddress address);
	void stop();


	//////////////////////////  CommsCarrier internal interface methods
protected:
	void setAddressImp(NetworkAddress address) Q_DECL_OVERRIDE;
	bool setStartImp(bool) Q_DECL_OVERRIDE;

	bool isStartedImp() const Q_DECL_OVERRIDE;

	qint64 writeDataImp(const QByteArray &datagram, const NetworkAddress &address) Q_DECL_OVERRIDE;
	qint64 readDataImp(char *data, qint64 maxlen, QHostAddress *host = nullptr, quint16 *port = nullptr) Q_DECL_OVERRIDE;

	bool hasPendingDataImp() Q_DECL_OVERRIDE ;
	qint64 pendingDataSizeImp() Q_DECL_OVERRIDE ;

	QString errorStringImp() Q_DECL_OVERRIDE ;

	NetworkAddress addressImp() Q_DECL_OVERRIDE ;


	quint64 minimalPacketIntervalImp() Q_DECL_OVERRIDE ;
	quint64	maximalPacketIntervalImp() Q_DECL_OVERRIDE ;


	// FOR REFERENCE, CommsCarrier interface signals:
	/*
	signals:

	// Send this signal when there is new data ready for reading
	void carrierReadyRead();
	void carrierError(QString string);
	void carrierSendingOpportunity(quint64 now);
	void carrierConnectionStatusChanged(bool connected);
	*/
};


#endif
// MOCKCOMMSCARRIER_HPP
