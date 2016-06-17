#include "AgentNameGenerator.hpp"


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
	, "aes"
	, "anim"
	, "apoll"
	, "imac"
	, "educ"
	, "equis"
	, "extr"
	, "guius"
	, "hann"
	, "equi"
	, "amora"
	, "hum"
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




QString AgentNameGenerator::generate (){
	QString out;
	while(out.size()<=0){
		out.append(namePrefix[rand()%(sizeof(namePrefix)/sizeof(char *))]);
		out.append(nameStem[rand()%(sizeof(nameStem)/sizeof(char *))]);
		out.append(nameSuffix[rand()%(sizeof(nameSuffix)/sizeof(char *))]);
	}
	out[0]=out[0].toUpper();
	return out;
}
