#ifndef DISCOVERYCOURIER_HPP
#define DISCOVERYCOURIER_HPP

#include "Courier.hpp"

#include <QList>
#include <QMap>
#include <QString>
#include <QVariantMap>

class DiscoveryParticipant;

class DiscoveryCourier : public Courier
{
		Q_OBJECT

	private:
		DiscoveryParticipant &mPart;
		QVariantMap mData;

	public:
		explicit DiscoveryCourier(DiscoveryParticipant &part, QObject *parent = nullptr);
		virtual ~DiscoveryCourier();


	public:



		//Let the CommChannel know what we want
		CourierMandate mandate() override;

		//Override to act on sending opportunity.
		//Return nubmer of bytes sent ( >0 ) if you took advantage of the opportunity
		quint16 sendingOpportunity(QDataStream &ds) override;

		//Override to act on data received
		//Return number of bytes actually read.
		quint16 dataReceived(QDataStream &ds, quint16 availableBytes) override;

	signals:

		void authenticationUpdate();

};

#endif // DISCOVERYCOURIER_HPP
