#include "NameGenerator.hpp"

NameGenerator::NameGenerator()
{

}


QString prefixes[]={
	"", //who said we need to add a prefix?
	"bel", //lets say that means "the good"
	"nar", //"The not so good as Bel"
	"xan", //"The evil"
	"bell", //"the good"
	"natr", //"the neutral/natral"
	"ev", //Man am I original
};

QString stems[]= {
	"adur", "aes", "anim", "apoll", "imac",
	"educ", "equis", "extr", "guius", "hann",
	"equi", "amora", "hum", "iace", "ille",
	"inept", "iuv", "obe", "ocul", "orbis"
};



QString suffixes[]={
	"", "us", "ix", "ox", "ith",
	"ath", "um", "ator", "or", "axia",
	"imus", "ais", "itur", "orex", "o",
	"y"
};



QString NameGenerator::generate(){
	QString out;
	out+=prefixes[rand()%7];
	out+=stems[rand()%20];
	out+=suffixes[rand()%16];
	return out;
}
