#ifndef MTLOBEFACTORY_HPP
#define MTLOBEFACTORY_HPP

#include <QString>

class Mtlobe;

class MtlobeFactory
{
public:
	MtlobeFactory();

public:
	Mtlobe *mtlobeFactory(QString type);
};

#endif
// MTLOBEFACTORY_HPP
