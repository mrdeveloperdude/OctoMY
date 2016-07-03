#ifndef SENSORSCOURIER_HPP
#define SENSORSCOURIER_HPP
#include "Courier.hpp"
#include "comms/messages/SensorsMessage.hpp"

#include <QObject>

#include <QGeoPositionInfo>
class QCompassReading;
class QAccelerometerReading;
class QGyroscopeReading;


class SensorsCourier : public Courier{
		Q_OBJECT
	private:
		CourierMandate man;
		SensorsMessage msg;
	public:
		explicit SensorsCourier(QObject *parent = nullptr);

	public:
		//Let the CommChannel know what we want
		CourierMandate mandate() override;

		//Override to acton sending opportunity.
		//Return true if you took advantage of the opportunity
		quint16 sendingOpportunity(QDataStream &ds) override;

	private slots:
		void onPositionUpdated(const QGeoPositionInfo &info);
		void onCompassUpdated(QCompassReading *);
		void onAccelerometerUpdated(QAccelerometerReading *);
		void onGyroscopeUpdated(QGyroscopeReading *);

};

#endif // SENSORSCOURIER_HPP
