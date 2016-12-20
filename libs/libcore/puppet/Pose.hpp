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

	Pose(quint32 size=0);

public:
	quint64 size() const;

	qreal value(quint32 index) const ;
	void setValue(quint32 index, qreal value);

	void mix(const Pose &other,qreal alpha);

	QString toString() const;


	QDataStream &receive(QDataStream &);
	QDataStream &send(QDataStream &) const;


};


class QDataStream;

QDataStream &operator<<(QDataStream &, const Pose &);
QDataStream &operator>>(QDataStream &, Pose &);




#endif // POSE_HPP
