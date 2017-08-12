#ifndef MOCKCOMMSCARRIER_HPP
#define MOCKCOMMSCARRIER_HPP


#include "comms/CommsCarrier.hpp"

#include <QObject>
#include <QUdpSocket>

class MockCommsCarrier: public CommsCarrier
{
	Q_OBJECT
private:
	QByteArray mMockData;
public:
	explicit MockCommsCarrier(QObject *parent=nullptr);
	virtual ~MockCommsCarrier();

	// Mock interface
public:

	void writeMock(QByteArray data);
	QByteArray readMock();

	// CommsCarrier internal interface methods
protected:

	bool startImp(NetworkAddress address) Q_DECL_OVERRIDE;
	void stopImp() Q_DECL_OVERRIDE;

	bool isStartedImp() const Q_DECL_OVERRIDE;

	qint64 writeDataImp(const QByteArray &datagram, const NetworkAddress &address) Q_DECL_OVERRIDE;
	qint64 readDataImp(char *data, qint64 maxlen, QHostAddress *host = nullptr, quint16 *port = nullptr) Q_DECL_OVERRIDE;

	bool hasPendingDataImp() Q_DECL_OVERRIDE ;
	qint64 pendingDataSizeImp() Q_DECL_OVERRIDE ;

	QString errorStringImp() Q_DECL_OVERRIDE ;

	NetworkAddress addressImp() Q_DECL_OVERRIDE ;


	quint64 minimalPacketIntervalImp() Q_DECL_OVERRIDE ;
	quint64	maximalPacketIntervalImp() Q_DECL_OVERRIDE ;

};



#endif // MOCKCOMMSCARRIER_HPP
