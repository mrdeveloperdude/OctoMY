#include "RemoteNameGenerator.hpp"



static const QStringList remoteNamePrefix{
	"able"
	, "able_bodied"
	, "acidic"
	, "active"
	, "aggressive"
	, "agreeable"
	, "alive"
	, "ambitious"
	, "angry"
	, "ashy"
	, "athletic"
	, "attractive"
	, "bald"
	, "beautiful"
	, "better"
	, "bewildered"
	, "big"
	, "bitter"
	, "black"
	, "blue"
	, "brave"
	, "breezy"
	, "broad"
	, "bumpy"
	, "calm"
	, "capable"
	, "careful"
	, "chilly"
	, "chubby"
	, "clean"
	, "clever"
	, "clumsy"
	, "cold"
	, "colossal"
	, "cool"
	, "crashing"
	, "creamy"
	, "crooked"
	, "cuddly"
	, "curved"
	, "damaged"
	, "damp"
	, "dazzling"
	, "dead"
	, "deafening"
	, "deep"
	, "defeated"
	, "delicious"
	, "delightful"
	, "dirty"
	, "disgusting"
	, "drab"
	, "dry"
	, "durable"
	, "eager"
	, "easy"
	, "echoing"
	, "elegant"
	, "embarrassed"
	, "enduring"
	, "extraordinary"
	, "extravagant"
	, "energetic"
	, "faint"
	, "faithful"
	, "famous"
	, "fancy"
	, "fat"
	, "fierce"
	, "firm"
	, "fit"
	, "fixed"
	, "flabby"
	, "flabbergasted"
	, "flaky"
	, "flat"
	, "fluffy"
	, "forceful"
	, "forcible"
	, "freezing"
	, "fresh"
	, "gentle"
	, "gifted"
	, "gigantic"
	, "glamorous"
	, "gorgeous"
	, "gray"
	, "greasy"
	, "great"
	, "green"
	, "grumpy"
	, "hale"
	, "hallowed"
	, "handsome"
	, "happy"
	, "hardy"
	, "harsh"
	, "hearty"
	, "heavy"
	, "heavy_duty"
	, "helpful"
	, "helpless"
	, "high"
	, "hissing"
	, "hollow"
	, "hot"
	, "howling"
	, "huge"
	, "icy"
	, "immense"
	, "important"
	, "inexpensive"
	, "itchy"
	, "jealous"
	, "jolly"
	, "juicy"
	, "kind"
	, "large"
	, "lazy"
	, "lemon"
	, "little"
	, "lively"
	, "long"
	, "livid"
	, "loose"
	, "loud"
	, "low"
	, "magnificent"
	, "mammoth"
	, "mango"
	, "massive"
	, "mealy"
	, "melodic"
	, "melted"
	, "microscopic"
	, "mighty"
	, "miniature"
	, "moldy"
	, "muscular"
	, "mushy"
	, "mysterious"
	, "narrow"
	, "nervous"
	, "nice"
	, "noisy"
	, "nutritious"
	, "nutty"
	, "obedient"
	, "obnoxious"
	, "odd"
	, "orange"
	, "panicky"
	, "petite"
	, "pitiful"
	, "plain"
	, "plump"
	, "polite"
	, "poor"
	, "powerful"
	, "prickly"
	, "proud"
	, "puny"
	, "purple"
	, "purring"
	, "pulsating"
	, "putrid"
	, "petrified"
	, "quaint"
	, "quiet"
	, "rancid"
	, "rapping"
	, "raspy"
	, "red"
	, "refined"
	, "reinforced"
	, "repulsive"
	, "rhythmic"
	, "rich"
	, "ripe"
	, "robust"
	, "rotten"
	, "rough"
	, "round"
	, "rugged"
	, "salmon"
	, "salty"
	, "savory"
	, "scary"
	, "scrawny"
	, "screeching"
	, "scruffy"
	, "secure"
	, "shaggy"
	, "shallow"
	, "shapely"
	, "sharp"
	, "short"
	, "shrilling"
	, "shy"
	, "silly"
	, "sinewy"
	, "skinny"
	, "slimy"
	, "small"
	, "solid"
	, "sound"
	, "sour"
	, "spicy"
	, "spoiled"
	, "square"
	, "squeaking"
	, "stable"
	, "stale"
	, "stalwart"
	, "stark"
	, "staunch"
	, "steady"
	, "steep"
	, "sticky"
	, "stocky"
	, "stout"
	, "straight"
	, "strapping"
	, "strong"
	, "sturdy"
	, "substantial"
	, "sweet"
	, "tall"
	, "tangy"
	, "tart"
	, "tasteless"
	, "tasty"
	, "teeny"
	, "tenacious"
	, "tender"
	, "thankful"
	, "thoughtless"
	, "thundering"
	, "tight"
	, "tinkling"
	, "tiny"
	, "tough"
	, "ugly"
	, "uneven"
	, "unimportant"
	, "uninterested"
	, "unkempt"
	, "unsightly"
	, "unyielding"
	, "uptight"
	, "vast"
	, "victorious"
	, "vigorous"
	, "wailing"
	, "warm"
	, "weak"
	, "wet"
	, "whining"
	, "whispering"
	, "white"
	, "wide"
	, "witty"
	, "wonderful"
	, "wooden"
	, "worried"
	, "wrong"
	, "yellow"
	, "yummy"
	, "zealous"

};


static const QStringList remoteNameStem {
	"_"
};


static const  QStringList remoteNameSuffix{

	"affector"
	, "authority"
	, "conductor"
	, "determination"
	, "direction"
	, "director"
	, "discipline"
	, "domination"
	, "dominator"
	, "dominion"
	, "driver"
	, "effector"
	, "force"
	, "enforcer"
	, "government"
	, "governor"
	, "guidance"
	, "guide"
	, "limitation"
	, "limiter"
	, "management"
	, "manager"
	, "manipulation"
	, "manipulator"
	, "might"
	, "overseer"
	, "oversight"
	, "predomination"
	, "predominator"
	, "qualification"
	, "qualifier"
	, "regimentation"
	, "regimentor"
	, "regulation"
	, "regulator"
	, "restrainer"
	, "restraint"
	, "restriction"
	, "restrictor"
	, "rule"
	, "ruler"
	, "subjection"
	, "subjector"
	, "subordination"
	, "subordinator"
	, "superintendence"
	, "superintender"
	, "superintendant"
	, "superviser"
	, "supervision"

};





RemoteNameGenerator::RemoteNameGenerator()
	: NameGenerator(remoteNamePrefix, remoteNameStem, remoteNameSuffix)
{

}

RemoteNameGenerator::~RemoteNameGenerator()
{

}
