#ifndef PERSONALITYCOLORS_HPP
#define PERSONALITYCOLORS_HPP

#include "identity/Personality.hpp"

#include <QColor>

class PersonalityColors
{

private:
	Personality mPersonality;

private:
	qreal mP1;
	qreal mP2;
	qreal mP3;
	QColor mBodyColorHigh;
	QColor mBodyColorLow;
	QColor mBackgroundColorHigh;
	QColor mBackgroundColorLow;
	QColor mLimbColor;

public:
	PersonalityColors(QString id="");

public:
	qreal p1() const;
	qreal p2() const;
	qreal p3() const;
	QColor bodyColorHigh() const;
	QColor bodyColorLow() const;
	QColor backgroundColorHigh() const;
	QColor backgroundColorLow() const;
	QColor limbColor() const;


public:
	void setID(QString id);

	Personality personality();

};


#endif // PERSONALITYCOLORS_HPP
