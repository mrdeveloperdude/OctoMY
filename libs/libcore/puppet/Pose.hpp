#ifndef POSE_HPP
#define POSE_HPP

#include <QtGlobal>
#include <QString>

class Pose
{
public:

	const static quint64 MAX_SIZE=4;
public:
	bool ok=false;
	int pos1=0;
	quint64 size()
	{
		return 4;
	}

	QString toString() const;

};


class QDataStream;

QDataStream &operator<<(QDataStream &, const Pose &);
QDataStream &operator>>(QDataStream &, Pose &);




#endif // POSE_HPP
