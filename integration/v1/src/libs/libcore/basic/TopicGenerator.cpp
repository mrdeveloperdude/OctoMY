#include "TopicGenerator.hpp"



static const QStringList topicPrefix{
	"Ethical look at "
	, "Tehcnical look at "
	, "Bird's eye view of "
	, "In-depth view of "
	, "Series on "
	, "Quick look at "
	, "Philosophical look at "
	, "Sensational look at "
	, "Reddit story about "
	, "Twitter story about "
	, "Blogger story about "
	, "Business look at "
	, "Market look at "
	, "Entrepeneurical look at "
	, "Controversial look at "
};


static const QStringList topicStem {
	"robots in general"
	, "humanoid robots"
	, "factory robots"
	, "military robots"
	, "medical robots"
	, "entertainment robots"
	, "fighting robots"
	, "DARPA Challenge"
	, "biped robots"
	, "hexapod robots"
	, "quadruped robots"
	, "android robots"
	, "robotic protecies"
	, "flying robots"
	, "driving robots"
	, "swimming robots"
	, "the uncanny valley of robotics"
	, "the race for power density in robotics"
	, "the engineering practices in robotics"
	, "the way robotics is thought"
	, "hobby robotics"
	, "submarine robotics"
	, "research robots"
	, "space robots"
	, "robots in Sci-Fi"
	, "robots"
	, "robotics"
	, "general AI"
	, "military robots"
	, "drones"
	, "covert robots"
	, "killer robots"
	, "assasination robots"
	, "automation robots"
	, "machining robots (CNC)"
	, "welding robots"
	, "self driving cars"
	, "self driving trains"
	, "self driving planes"
	, "self driving rockets"
	, "self driving space craft"
	, "nursery robots"
	, "nursing home robots"

};


static const  QStringList topicSuffix{
	" with special focus on viability"
	, " with special focus on politics"
	, " with special focus on scandals"
	, " with special focus on the hype"
	, " with special focus on how society views it"
	, " with special focus on the positive side effects"
	, " with special focus on the negative side effects"
	, " with special focus on how this impacts our society"
	, " with special focus on how this impacts our planet's ecosystem"
	, " with special focus on when this will likely occur"
	, " with special focus on the controvery surrounding it"
	, " with special focus on the people and companies behind it"
};





TopicGenerator::TopicGenerator()
	: NameGenerator(topicPrefix, topicStem, topicSuffix)
{

}

TopicGenerator::~TopicGenerator()
{

}
