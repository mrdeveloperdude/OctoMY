#ifndef COMMSCARRIERLOCAL_HPP
#define COMMSCARRIERLOCAL_HPP


#include <QObject>
#include <QLocalServer>
#include <QLocalSocket>

#include "CommsCarrier.hpp"

#include "comms/address/CarrierAddressLocal.hpp"

/**
 * @brief The CommsCarrierLocal class is an implementation of CommsCarrier that
 * uses QLocalSocket and QLocalServer as a quick, easy local transport.
 */
class CommsCarrierLocal: public CommsCarrier
{
	Q_OBJECT
protected:
	static const qint32 MAX_PAYLOAD_SIZE;

private:
	QSharedPointer<CarrierAddressLocal> mLocalAddress;
	
	QLocalServer mLocalServer;
	QLocalSocket *mLocalSocket{nullptr};
	QString mServerName;

public:
	explicit CommsCarrierLocal(QObject *parent=nullptr);
	virtual ~CommsCarrierLocal() override;

	// CommsCarrier internal interface methods
protected:
	void configureImp() override;
	bool activateImp(const bool on) override;

	void setAddressImp(QSharedPointer<CarrierAddress> address) override;

	bool isActiveImp() const override;

	qint64 writeDataImp(const QByteArray &datagram, QSharedPointer<CarrierAddress>address) override;
	qint64 readDataImp(char *data, qint64 maxlen, QHostAddress *host = nullptr, quint16 *port = nullptr) override;

	bool hasPendingDataImp() override ;
	qint64 pendingDataSizeImp() override ;

	QString errorStringImp() override ;

	QSharedPointer<CarrierAddress> addressImp() override ;

	quint64 minimalPacketIntervalImp() override ;
	quint64	maximalPacketIntervalImp() override ;

};


#endif // COMMSCARRIERLOCAL_HPP
