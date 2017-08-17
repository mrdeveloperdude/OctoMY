#ifndef PERSONALITY_HPP
#define PERSONALITY_HPP

#include <QString>

class RNG;

class Personality
{
private:
	QString mID;
	RNG *mRNG;
public:

public:
	Personality(QString id="");
	virtual ~Personality();

public:
	QString id() const;
	void reset();
	RNG &rng();
	Personality subPersonality(QString key);
};



#endif // PERSONALITY_HPP
