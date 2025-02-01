#ifndef MOCKCOMMSCARRIER_HPP
#define MOCKCOMMSCARRIER_HPP

#include "comms/carriers/CommsCarrier.hpp"

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
	QSharedPointer<CarrierAddress> mOurAddress;
	bool mOverrideStartStop;

public:
	explicit MockCommsCarrier(QObject *parent=nullptr);
	virtual ~MockCommsCarrier() override;

	////////////////////////// Mock interface
public:
	void mockWriteMock(QByteArray data, QSharedPointer<CarrierAddress> address, bool sendReadyReadSignal=true);
	QByteArray mockReadMock(QSharedPointer<CarrierAddress> address);
	void mockSetHasPendingData(bool hasPendingData);
	void mockSetWriteBatchSize(qint64 writeBatchSize=-1);
	void mockSetStartFail(bool startFail=true);
	void mockSetMinimalPacketInterval(quint64 size);
	void mockSetMaximalPacketIntervalImp(quint64 size);
	void mockSetAddress(QSharedPointer<CarrierAddress> addr);
	void mockSetOverrideSendingtimer(bool override);

	void mockStartSendingTimer();
	void mockStopSendingTimer();

	void mockTriggerReadyRead();
	void mockTriggerErrorSignal(QString error);
	void mockTriggerSendingOpportunity(quint64 now);
	void mockTriggerConnectionStatusChanged(bool connected);


	// CommsCarrier overrides to take countrol over sending opportunity timer
public slots:
	bool start(QSharedPointer<CarrierAddress> address);
	void stop();


	//////////////////////////  CommsCarrier internal interface methods
protected:

	void configureImp() override;
	bool activateImp(bool) override;

	void setAddressImp(QSharedPointer<CarrierAddress> address) override;


	bool isActiveImp() const override;

	qint64 writeDataImp(const QByteArray &datagram, QSharedPointer<CarrierAddress> address) override;
	qint64 readDataImp(char *data, qint64 maxlen, QHostAddress *host = nullptr, quint16 *port = nullptr) override;

	bool hasPendingDataImp() override ;
	qint64 pendingDataSizeImp() override ;

	QString errorStringImp() override ;

	QSharedPointer<CarrierAddress> addressImp() override ;


	quint64 minimalPacketIntervalImp() override ;
	quint64	maximalPacketIntervalImp() override ;


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
