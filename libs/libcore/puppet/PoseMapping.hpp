#ifndef POSEMAPPING_HPP
#define POSEMAPPING_HPP

#include <QVector>

class Pose;

class PoseMapping
{
private:
	QVector<quint32> mMapping;

public:
	explicit PoseMapping(quint32 size);
	virtual  ~PoseMapping();

public:
	void setMapping(quint32 from, quint32 to);
	quint32 map(quint32 from)const;
	qreal value(Pose &pose, quint32 from) const;
	QString toString() const;

	QDataStream &receive(QDataStream &);
	QDataStream &send(QDataStream &) const;


};


class QDataStream;

QDataStream &operator<<(QDataStream &, const Pose &);
QDataStream &operator>>(QDataStream &, Pose &);


#endif // POSEMAPPING_HPP
