#include "GenderGenerator.hpp"


GenderGenerator::GenderGenerator()
	: rng(RNG::sourceFactory("devu"))
{
	alternatives<<"Male"<<"Female"<<"Genderless";
}

GenderGenerator::~GenderGenerator()
{
	delete rng;
	rng=nullptr;
}

QString GenderGenerator::generate()
{
	QString gender="Genderless";
	if(nullptr!=rng) {
		gender=alternatives[static_cast<int>(rng->generateInt32())%alternatives.size()];
	}
	return gender;
}

