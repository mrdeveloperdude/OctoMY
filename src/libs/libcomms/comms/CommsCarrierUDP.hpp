#ifndef COMMSCARRIERUDP_HPP
#define COMMSCARRIERUDP_HPP

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
	virtual ~CommsCarrierUDP() Q_DECL_OVERRIDE;


private slots:
	void onReadyRead();
	void onError(QAbstractSocket::SocketError);


	// CommsCarrier internal interface methods
protected:
	void configureImp() Q_DECL_OVERRIDE;
	bool activateImp(const bool on) Q_DECL_OVERRIDE;

	void setAddressImp(NetworkAddress address) Q_DECL_OVERRIDE;

	bool isActiveImp() const Q_DECL_OVERRIDE;

	qint64 writeDataImp(const QByteArray &datagram, const NetworkAddress &address) Q_DECL_OVERRIDE;
	qint64 readDataImp(char *data, qint64 maxlen, QHostAddress *host = nullptr, quint16 *port = nullptr) Q_DECL_OVERRIDE;

	bool hasPendingDataImp() Q_DECL_OVERRIDE ;
	qint64 pendingDataSizeImp() Q_DECL_OVERRIDE ;

	QString errorStringImp() Q_DECL_OVERRIDE ;

	NetworkAddress addressImp() Q_DECL_OVERRIDE ;


	quint64 minimalPacketIntervalImp() Q_DECL_OVERRIDE ;
	quint64	maximalPacketIntervalImp() Q_DECL_OVERRIDE ;



};

#endif
// COMMSCARRIERUDP_HPP
