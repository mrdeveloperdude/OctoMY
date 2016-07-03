#ifndef DISCOVERYCOURIER_HPP
#define DISCOVERYCOURIER_HPP

#include "Courier.hpp"

#include <QList>
#include <QMap>
#include <QString>

class DiscoveryParticipant;

class DiscoveryCourier : public Courier
{
		Q_OBJECT

	private:
		QMap<QString, DiscoveryParticipant *> factors;
		QList<DiscoveryParticipant *> order;

	public:
		explicit DiscoveryCourier(QObject *parent = 0);


	public:

		void registerDiscoveryAuthentication(DiscoveryParticipant &);

		//Let the CommChannel know what we want
		CourierMandate mandate() override;

		//Override to act on sending opportunity.
		//Return nubmer of bytes sent ( >0 ) if you took advantage of the opportunity
		quint16 sendingOpportunity(QDataStream &ds) override;

};

#endif // DISCOVERYCOURIER_HPP
