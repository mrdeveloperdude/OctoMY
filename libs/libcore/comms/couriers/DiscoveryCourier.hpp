#ifndef DISCOVERYCOURIER_HPP
#define DISCOVERYCOURIER_HPP

#include "Courier.hpp"
#include "basic/NodeAssociate.hpp"

#include <QList>
#include <QMap>
#include <QString>
#include <QVariantMap>
#include <QSharedPointer>

/*!
 * \brief The DiscoveryCourier class is used by DiscoveryClient to test direct
 * connections and to exchange security information so that peers may trust
 * eachoter
 */

class DiscoveryCourier : public Courier
{
	Q_OBJECT

private:
	QSharedPointer<NodeAssociate> mAss;
	QVariantMap mData;
	quint64 mLastSend;

public:
	explicit DiscoveryCourier(QSharedPointer<NodeAssociate> ass, QObject *parent = nullptr);
	virtual ~DiscoveryCourier();

// Courier interface
public:

	//Let the CommChannel know what we want
	CourierMandate mandate() const Q_DECL_OVERRIDE;

	//Override to act on sending opportunity.
	//Return nubmer of bytes sent ( >0 ) if you took advantage of the opportunity
	quint16 sendingOpportunity(QDataStream &ds) Q_DECL_OVERRIDE;

	//Override to act on data received
	//Return number of bytes actually read.
	quint16 dataReceived(QDataStream &ds, quint16 availableBytes) Q_DECL_OVERRIDE;

signals:

	void authenticationUpdate();

};

#endif // DISCOVERYCOURIER_HPP
