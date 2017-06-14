#ifndef LOCUSFACTORY_HPP
#define LOCUSFACTORY_HPP

#include <QString>

class Locus;

class LocusFactory
{
public:
	LocusFactory();
public:
	Locus *locusFactory(QString type);
};

#endif // LOCUSFACTORY_HPP
