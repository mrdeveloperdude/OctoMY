#ifndef POSEMAPPING_HPP
#define POSEMAPPING_HPP

#include <QVector>
#include <QString>
#include <QVariantList>

class Pose;

class PoseMapping
{
private:
	QVector<quint32> mMapping;
	QVector<QString> mNames;

public:
	explicit PoseMapping(quint32 size=0);
	virtual  ~PoseMapping();

public:
	void setMapping(quint32 from, quint32 to, bool swap=false);
	void setName(quint32 from, QString name);
	quint32 map(quint32 from)const;
	qreal value(Pose &pose, quint32 from) const;
	QString name(quint32 from) const;
	quint32 size() const;
	QString toString() const;

	void resize(quint32 );

	QDataStream &receive(QDataStream &);
	QDataStream &send(QDataStream &) const;

	QVariantList toMap() const;
	void fromMap( QVariantList );

	//Copy data from other instance
	void set(const PoseMapping &other);

};


class QDataStream;

QDataStream &operator<<(QDataStream &, const PoseMapping &);
QDataStream &operator>>(QDataStream &, PoseMapping &);


#endif // POSEMAPPING_HPP
