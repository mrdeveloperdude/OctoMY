#include "AgentNameGenerator.hpp"



static const QStringList agentNamePrefix{
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


static const QStringList agentNameStem {
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


static const  QStringList agentNameSuffix{
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
	: NameGenerator(agentNamePrefix, agentNameStem, agentNameSuffix)
{

}


AgentNameGenerator::~AgentNameGenerator()
{

}
