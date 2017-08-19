#ifndef PERSONALITY_HPP
#define PERSONALITY_HPP

#include <QString>

class RNG;



class Personality
{
public:
	static const QString COLORS;
	static const QString IDENTICON;
	static const QString IRIS;

private:
	QString mID;
	RNG *mRNG;

public:
	Personality(QString id="");
	Personality(QString id, QString key);


	Personality( const Personality& other );

	virtual ~Personality();


public:

	void setID(QString id, QString key);
	QString id() const;
	void reset();
	RNG &rng();
	Personality subPersonality(QString key);
};



#endif // PERSONALITY_HPP
