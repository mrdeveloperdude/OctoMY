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
		quint64 sendingOpportunity(QDataStream &ds, quint32 availableBytes) override;

};


#endif // DIRECTPOSECOURIER_HPP
