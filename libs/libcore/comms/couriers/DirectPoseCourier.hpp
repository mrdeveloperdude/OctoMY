#ifndef DIRECTPOSECOURIER_HPP
#define DIRECTPOSECOURIER_HPP

#include "Courier.hpp"
#include "puppet/Pose.hpp"

class DirectPoseCourier: public Courier
{
	Q_OBJECT
private:
	quint64 lastRX;
	quint64 lastTX;
	Pose pose;
public:
	explicit DirectPoseCourier(QObject *parent=0);

public:
	void setPose(Pose &);



	// Courier interface
public:


	// Let the CommChannel know what we want
	CourierMandate mandate() const Q_DECL_OVERRIDE;

	// Override to act on sending opportunity.
	// Return nubmer of bytes sent ( >0 ) if you took advantage of the opportunity
	quint16 sendingOpportunity(QDataStream &ds) Q_DECL_OVERRIDE;


};


#endif // DIRECTPOSECOURIER_HPP
