#include "GenderGenerator.hpp"

#include "uptime/MethodGate.hpp"


GenderGenerator::GenderGenerator()
	: rng(RNG::sourceFactory("devu"))
{
	OC_METHODGATE();
	alternatives<<"Male"<<"Female"<<"Genderless";
}

GenderGenerator::~GenderGenerator()
{
	OC_METHODGATE();
	delete rng;
	rng=nullptr;
}

QString GenderGenerator::generate()
{
	OC_METHODGATE();
	QString gender="Genderless";
	if(nullptr!=rng) {
		const int sz=alternatives.size();
		const int index=(static_cast<int>(rng->generateInt32())) % sz;
		gender=alternatives[index];
	}
	return gender;
}

