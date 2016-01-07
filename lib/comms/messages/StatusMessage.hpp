#ifndef STATUSMESSAGE_HPP
#define STATUSMESSAGE_HPP

#include <QDataStream>
#include <QVector3D>

class StatusMessage{
	private:

		qint64 timestamp;
		QVector3D gps;
		QVector3D slam;
		QVector3D gyroscope;
		QVector3D accellerometer;
		qint32 compass;
		qint32 temperature;

	public:
		StatusMessage(
				const qint64 timestamp
				,const QVector3D gps
				,const QVector3D slam
				,const QVector3D gyroscope
				,const QVector3D accellerometer
				,const qint32 compass
				,const qint32 temperature
				);

		QDataStream& operator<<(QDataStream&);
		QDataStream& operator>>(QDataStream &);
};

#endif // STATUSMESSAGE_HPP
