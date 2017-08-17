#ifndef IRISRENDRER_HPP
#define IRISRENDRER_HPP

#include "identity/Personality.hpp"

#include "Simplex.hpp"

#include <QColor>

class QRect;
class QPainter;

class IrisRendrer
{
private:
	Personality mPersonality;
	Simplex<qreal> mSimplexStreaks;
	Simplex<qreal> mSimplexRipples;
	qreal oval=0.0;
	qreal pupilSize=1.0;
	qreal edgeSize=1.0;
	qreal ringRes=1.0;
	qreal ringStartRadius=1.0;
	qreal contrast=1.0;
	qreal colorFollow=1.0;
	qreal baseX=0.0;
	qreal baseY=0.0;

	qreal exposure=1.0;
	qreal cutoff=0.025;


	QRgb c1=QColor("#8294aa").rgb();
	QRgb c2=QColor("#752e0b").rgb();

	QRgb* pixels=nullptr;

	qreal* pixelsF=nullptr;
public:
	IrisRendrer(Personality &p);

public:

	void draw(QRect &rect, QPainter &paint);

	void setParameter(QString name, qreal value);
};

#endif // IRISRENDRER_HPP
