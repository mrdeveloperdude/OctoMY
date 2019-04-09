#ifndef GENDERGENERATOR_HPP
#define GENDERGENERATOR_HPP

#include "random/RNG.hpp"

class RNG;

class GenderGenerator
{

private:
	RNG *rng;
	QStringList alternatives;

public:
	explicit GenderGenerator();
	virtual ~GenderGenerator();

public:
	QString generate();

};
#endif
// GENDERGENERATOR_HPP
