#ifndef IDENTICON_HPP
#define IDENTICON_HPP

#include "security/PortableID.hpp"

#include <QtGlobal>
#include <QDomDocument>
#include <QtGui/QPixmap>
#include <QByteArray>

class RNG;

class Identicon
{

private:
	QByteArray data;
	bool dirty;
	PortableID mID;
	QDomDocument doc;
	RNG *rng;

	QColor mBodyColorHigh;
	QColor mBodyColorLow;
	QColor mBackgroundColorHigh;
	QColor mBackgroundColorLow;
	QColor mLimbColor;

public:
	explicit Identicon(PortableID &id);
	explicit Identicon();
	virtual  ~Identicon();

private:

	float frand();

	void erectLimb(QDomElement &o, QString limbStyle, bool top, bool left, bool mid,float p1,float p2,float p3,float p4,float p5,float p6);

public:
	//void setSvgURL(QString);
	void setPortableID(PortableID id);

	QDomDocument domDocument();

	QPixmap pixmap(qint32 w=-1,qint32 h=-1,qreal zoom=0.0);
	QImage image(qint32 w=-1,qint32 h=-1,qreal zoom=0.0);

	//QColor

	QColor bodyColorHigh()
	{
		return mBodyColorHigh;
	}

	QColor bodyColorLow()
	{
		return mBodyColorLow;
	}


	QColor backgroundColorHigh()
	{
		return mBackgroundColorHigh;
	}


	QColor backgroundColorLow()
	{
		return mBackgroundColorLow;
	}


	QColor limbColor()
	{
		return mLimbColor;
	}




private:

	void regenerateIdenticon();
};

#endif // IDENTICON_HPP
