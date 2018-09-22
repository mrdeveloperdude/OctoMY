#include "NameGenerator.hpp"

#include "rng/RNG.hpp"

#include <QString>


NameGenerator::NameGenerator(const QStringList namePrefix, const QStringList nameStem, const QStringList nameSuffix)
	: rng(RNG::sourceFactory("devu"))
	, mNamePrefix(namePrefix)
	, mNameStem(nameStem)
	, mNameSuffix(nameSuffix)
{

}


NameGenerator::~NameGenerator()
{
	delete(rng);
	rng=nullptr;
}



quint32 NameGenerator::randomInt()
{
	if(nullptr==rng || !rng->isOk()) {
		return static_cast<quint32>(::rand());
	} else {
		return rng->generateInt32();
	}
}

QString NameGenerator::generate ()
{
	QString out;
	while(out.size()<=0) {
		out.append(mNamePrefix[randomInt()%mNamePrefix.size()]);
		out.append(mNameStem[randomInt()%mNameStem.size()]);
		out.append(mNameSuffix[randomInt()%mNameSuffix.size()]);
	}
	//out[0]=out[0].toUpper();
	return out;
}

