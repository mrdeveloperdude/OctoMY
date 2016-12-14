#ifndef POSE_HPP
#define POSE_HPP

#include <QtGlobal>
#include <QString>
#include <QVector>


class Pose
{
public:

	static const quint32 MAX_SIZE;

private:

	QVector<qreal> mValues;

public:

	Pose(quint32 size);

public:
	quint64 size();

	QString toString() const;


	QDataStream &receive(QDataStream &);
	QDataStream &send(QDataStream &) const;


};


class QDataStream;

QDataStream &operator<<(QDataStream &, const Pose &);
QDataStream &operator>>(QDataStream &, Pose &);




#endif // POSE_HPP
