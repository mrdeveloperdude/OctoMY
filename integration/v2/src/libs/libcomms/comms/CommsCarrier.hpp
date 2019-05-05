#ifndef COMMSCARRIER_HPP
#define COMMSCARRIER_HPP

#include "NetworkAddress.hpp"

#include "RateCalculator.hpp"

#include "uptime/ConfigureHelper.hpp"

#include <QObject>
#include <QTimer>

class AddressList;
class QHostAddress;

/**
 * @brief The CommsCarrier class is the base for the data carriers availabler to CommsChannel.
 *
 * The job of the carrier is to perform RX/TX of data buffers to and from designated carrier-specific addresses.
 *
 * The most importnat carrier subclass is CommsCarrierUDP which imlpements transport over UDP/IP networks.
 *
 */
class CommsCarrier: public QObject
{
	Q_OBJECT
private:
	//Receive counter used in debug messages to make sense of the order of things
	static quint64 sTotalRxCount;
	static quint64 sTotalTxCount;

	// Are we connected?
	// NOTE: Connected is a relative term. It simply means "a package was successfully transmitted within some time limit"
	bool mConnected;

	ConfigureHelper mConfigureHelper;

protected:
	// The timer used to schedule when packets are sent to the other side
	QTimer mSendingTimer;

	RateCalculator mRXRate;
	RateCalculator mTXRate;
	RateCalculator mTXOpportunityRate;

public:
	explicit CommsCarrier(QObject *parent=nullptr);
	virtual ~CommsCarrier();

public:
	void configure();
	bool activate(bool on);
	bool connect(bool on);

private:
	void detectConnectionChanges(const quint64 now);


private slots:
	void onOpportunityTimer();


	// CommsCarrier external interface methods
public slots:
	quint64 connectionTimeout();

	void setHookCarrierSignals(QObject &ob, bool hook);

	void setListenAddress(NetworkAddress address);

	bool isStarted() const;
	bool isConnected() const;

	qint64 writeData(const QByteArray &datagram, const NetworkAddress &address);
	qint64 readData(char *data, qint64 maxlen, QHostAddress *host = nullptr, quint16 *port = nullptr);

	bool hasPendingData();
	qint64 pendingDataSize();

	QString errorString();

	NetworkAddress address();

	quint64 minimalPacketInterval();
	quint64 maximalPacketInterval();

	quint64 setDesiredOpportunityInterval(quint64 interval);

	quint64 opportunityInterval();


// CommsCarrier internal interface methods
protected:
	virtual void configureImp() =0;
	virtual bool activateImp(const bool on) =0;

	virtual void setAddressImp(NetworkAddress address) =0;

	virtual bool isActiveImp() const =0 ;

	virtual qint64 writeDataImp(const QByteArray &datagram, const NetworkAddress &address) =0 ;
	virtual qint64 readDataImp(char *data, qint64 maxlen, QHostAddress *host = nullptr, quint16 *port = nullptr) =0 ;

	virtual bool hasPendingDataImp() =0 ;
	virtual qint64 pendingDataSizeImp() =0 ;

	virtual QString errorStringImp() =0 ;

	virtual NetworkAddress addressImp() =0;

	virtual quint64 minimalPacketIntervalImp() =0;
	virtual quint64 maximalPacketIntervalImp() =0;

	// CommsCarrier interface signals
signals:

	// Send this signal when there is new data ready for reading
	void carrierReadyRead();
	void carrierError(QString string);
	void carrierSendingOpportunity(quint64 now);
	void carrierConnectionStatusChanged(bool connected);

};

#endif
// COMMSCARRIER_HPP
