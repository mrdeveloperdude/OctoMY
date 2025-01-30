#ifndef COMMSCARRIERUDP_H
#define COMMSCARRIERUDP_H

#include "CommsCarrier.hpp"

#include <QObject>
#include <QUdpSocket>

class CommsCarrierUDP: public CommsCarrier
{
	Q_OBJECT

protected:
	static const qint32 MAX_UDP_PAYLOAD_SIZE;

private:
	QUdpSocket mUDPSocket;
	NetworkAddress mLocalAddress;

public:
	explicit CommsCarrierUDP(QObject *parent=nullptr);
	virtual ~CommsCarrierUDP() override;

private slots:
	void onReadyRead();
	void onError(QAbstractSocket::SocketError);

	// CommsCarrier internal interface methods
protected:
	void configureImp() override;
	bool activateImp(const bool on) override;

	void setAddressImp(NetworkAddress address) override;

	bool isActiveImp() const override;

	qint64 writeDataImp(const QByteArray &datagram, const NetworkAddress &address) override;
	qint64 readDataImp(char *data, qint64 maxlen, QHostAddress *host = nullptr, quint16 *port = nullptr) override;

	bool hasPendingDataImp() override ;
	qint64 pendingDataSizeImp() override ;

	QString errorStringImp() override ;

	NetworkAddress addressImp() override ;

	quint64 minimalPacketIntervalImp() override ;
	quint64	maximalPacketIntervalImp() override ;

};

#endif
// COMMSCARRIERUDP_HPP
