#ifndef SENSORSCOURIER_HPP
#define SENSORSCOURIER_HPP
#include "Courier.hpp"
#include "comms/messages/StatusMessage.hpp"

#include <QObject>

#include <QGeoPositionInfo>
class QCompassReading;
class QAccelerometerReading;
class QGyroscopeReading;


class SensorsCourier : public Courier{
		Q_OBJECT
	private:
		CourierMandate man;
		StatusMessage msg;
	public:
		explicit SensorsCourier(QObject *parent = 0);

	public:
		//Let the CommChannel know what we want
		CourierMandate mandate() override;

		//Override to acton sending opportunity.
		//Return true if you took advantage of the opportunity
		quint64 sendingOportunity(QDataStream &ds, quint32 availableBytes) override;

	private slots:
		void onPositionUpdated(const QGeoPositionInfo &info);
		void onCompassUpdated(QCompassReading *);
		void onAccelerometerUpdated(QAccelerometerReading *);
		void onGyroscopeUpdated(QGyroscopeReading *);

};

#endif // SENSORSCOURIER_HPP
