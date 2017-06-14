#ifndef AGENTNAMEGENERATOR_HPP
#define AGENTNAMEGENERATOR_HPP

#include <QString>


class RNG;

class AgentNameGenerator{
	private:
		RNG *rng;

	private:

		static const char* namePrefix[];
		static const char* nameStem[];
		static const char* nameSuffix[];

	private:

		quint32 randomInt();

	public:

		explicit AgentNameGenerator();
		virtual ~AgentNameGenerator();

		QString generate ();

};



#endif // AGENTNAMEGENERATOR_HPP
