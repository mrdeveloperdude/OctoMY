/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

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
