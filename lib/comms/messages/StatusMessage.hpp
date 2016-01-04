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
		StatusMessage();

		QDataStream& operator<<(QDataStream&);
		QDataStream& operator>>(QDataStream &);
};

#endif // STATUSMESSAGE_HPP
