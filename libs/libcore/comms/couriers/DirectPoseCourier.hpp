#ifndef DIRECTPOSECOURIER_HPP
#define DIRECTPOSECOURIER_HPP

#include "Courier.hpp"
#include "puppet/Pose.hpp"

class DirectPoseCourier:public Courier{
		Q_OBJECT
	private:
		quint64 lastRX;
		quint64 lastTX;
		Pose pose;
	public:
		explicit DirectPoseCourier(QObject *parent=0);

	public:
		void setPose(Pose &);
		CourierMandate mandate() override;
		quint16 sendingOpportunity(QDataStream &ds) override;

};


#endif // DIRECTPOSECOURIER_HPP
