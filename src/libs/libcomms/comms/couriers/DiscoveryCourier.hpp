#ifndef DISCOVERYCOURIER_HPP
#define DISCOVERYCOURIER_HPP

#include "Courier.hpp"
#include "address/Associate.hpp"
#include "uptime/SharedPointerWrapper.hpp"
#include "utility/data/LoopingBuffer.hpp"

#include <QList>
#include <QMap>
#include <QString>
#include <QVariantMap>

/*!
 * \brief The DiscoveryCourier class is used by DiscoveryClient to test direct
 * connections and to exchange security information so that peers may trust
 * eachoter
 */

class DiscoveryCourier : public Courier
{
	Q_OBJECT

private:
	QSharedPointer<Associate> mAss;
	QVariantMap mData;
	quint64 mLastSend;
	static const QByteArray sLooperBuf;
	static const LoopingBuffer sLooper;

public:
	explicit DiscoveryCourier(QSharedPointer<Associate> ass, QSharedPointer<Comms> comms, QObject *parent = nullptr);
	virtual ~DiscoveryCourier() override;

// Courier interface
public:
	//Let the CommChannel know what we want
	CourierMandate mandate() const override;

	//Override to act on sending opportunity.
	//Return number of bytes sent ( >0 ) if you took advantage of the opportunity
	quint16 sendingOpportunity(QDataStream &ds) override;

	//Override to act on data received
	//Return number of bytes actually read.
	quint16 dataReceived(QDataStream &ds, quint16 availableBytes) override;

signals:
	void authenticationUpdate();

};

#endif
// DISCOVERYCOURIER_HPP
