#include "NameGenerator.hpp"

#include "random/RNG.hpp"
#include "uptime/MethodGate.hpp"

#include <QString>


NameGenerator::NameGenerator(const QStringList namePrefix, const QStringList nameStem, const QStringList nameSuffix)
	: rng(RNG::sourceFactory("devu"))
	, mNamePrefix(namePrefix)
	, mNameStem(nameStem)
	, mNameSuffix(nameSuffix)
{
	OC_METHODGATE();
}


NameGenerator::~NameGenerator()
{
	OC_METHODGATE();
	delete(rng);
	rng=nullptr;
}



quint32 NameGenerator::randomInt()
{
	OC_METHODGATE();
	if(nullptr==rng || !rng->isOk()) {
		return static_cast<quint32>(::rand());
	} else {
		return rng->generateInt32();
	}
}

QString NameGenerator::generate (bool capitalize)
{
	OC_METHODGATE();
	QString out;
	while(out.size() <= 0) {
		out.append(mNamePrefix[static_cast<int>(randomInt()%static_cast<unsigned int>(mNamePrefix.size()))]);
		out.append(mNameStem  [static_cast<int>(randomInt()%static_cast<unsigned int>(mNameStem.size()))]);
		out.append(mNameSuffix[static_cast<int>(randomInt()%static_cast<unsigned int>(mNameSuffix.size()))]);
	}
	if(capitalize){
		out[0]=out[0].toUpper();
	}
	return out;
}

