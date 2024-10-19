#ifndef NAMEGENERATOR_HPP
#define NAMEGENERATOR_HPP

#include <QStringList>

class RNG;


class NameGenerator
{
private:
	RNG *rng;

	const QStringList mNamePrefix;
	const QStringList mNameStem;
	const QStringList mNameSuffix;
public:
	explicit NameGenerator(const QStringList namePrefix, const QStringList nameStem, const QStringList nameSuffix);
	virtual ~NameGenerator();

private:
	quint32 randomInt();

public:

	QString generate (bool capitalize = false);
};

#endif // NAMEGENERATOR_HPP

