#include "AgentNameGenerator.hpp"

#include "random/RNG.hpp"

const char* AgentNameGenerator::namePrefix[] = {
	""
	, "bel"
	, "nar"
	, "xan"
	, "bell"
	, "natr"
	, "ev"
	, "ex"
	, "er"
	, "an"
	, "ax"
	, "al"
	, "ak"
	, "ac"
	, "ox"
	, "ol"
	, "ot"
	, "w"
	, "hog"
	, "hot"
	, "ro"
	, "ra"
	, "re"
	, "rah"
	, "max"
	, "min"
	, "oh"
};


const char* AgentNameGenerator::nameStem[] = {
	""
	, "adur"
	, "adun"
	, "ades"
	, "adis"
	, "adar"
	, "ader"
	, "aes"
	, "anim"
	, "apoll"
	, "imac"
	, "ina"
	, "ica"
	, "educ"
	, "equis"
	, "equi"
	, "extr"
	, "guius"
	, "hann"
	, "equi"
	, "amora"
	, "hum"
	, "post"
	, "pre"
	, "iace"
	, "ille"
	, "inept"
	, "iuv"
	, "obe"
	, "obi"
	, "alle"
	, "abe"
	, "ocul"
	, "orbis"
	, "obis"
	, "os"
	, "ob"
};


const char* AgentNameGenerator::nameSuffix[] = {
	""
	, "us"
	, "ix"
	, "ox"
	, "ith"
	, "ath"
	, "um"
	, "ator"
	, "or"
	, "axia"
	, "imus"
	, "ima"
	, "ais"
	, "itur"
	, "orex"
	, "o"
	, "y"
	, "ate"
	, "ath"
	, "eth"
};


AgentNameGenerator::AgentNameGenerator()
	: rng(RNG::sourceFactory("devu"))
{

}


AgentNameGenerator::~AgentNameGenerator(){
	delete(rng);
	rng=nullptr;
}

quint32 AgentNameGenerator::randomInt(){
	if(nullptr==rng){
		return ::rand();
	}
	else{
		return rng->generateInt32();
	}
}

QString AgentNameGenerator::generate (){
	QString out;
	while(out.size()<=0){
		out.append(namePrefix[randomInt()%(sizeof(namePrefix)/sizeof(char *))]);
		out.append(nameStem[randomInt()%(sizeof(nameStem)/sizeof(char *))]);
		out.append(nameSuffix[randomInt()%(sizeof(nameSuffix)/sizeof(char *))]);
	}
	//out[0]=out[0].toUpper();
	return out;
}

