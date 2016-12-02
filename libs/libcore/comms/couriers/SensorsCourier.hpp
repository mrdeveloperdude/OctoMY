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
		CourierMandate mMandate;
		SensorsMessage mMessage;
	public:
		SensorsCourier(QObject *parent = nullptr);

		// Courier interface
	public:
		//Let the CommChannel know what we want
		CourierMandate mandate() override;

		//Override to act on sending opportunity.
		//Return nubmer of bytes sent ( >0 ) if you took advantage of the opportunity
		quint16 sendingOpportunity(QDataStream &ds) override;

		//Override to act on data received
		//Return number of bytes actually read.
		quint16 dataReceived(QDataStream &ds, quint16 availableBytes) override;

	private slots:
		void onPositionUpdated(const QGeoPositionInfo &info);
		void onCompassUpdated(QCompassReading *);
		void onAccelerometerUpdated(QAccelerometerReading *);
		void onGyroscopeUpdated(QGyroscopeReading *);
		void onColorUpdated(QColor c);

};

#endif // SENSORSCOURIER_HPP
