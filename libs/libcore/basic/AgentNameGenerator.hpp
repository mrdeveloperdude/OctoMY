#ifndef AGENTNAMEGENERATOR_HPP
#define AGENTNAMEGENERATOR_HPP

#include <QString>


class AgentNameGenerator{
	private:

		static const char* namePrefix[];
		static const char* nameStem[];
		static const char* nameSuffix[];

	public:

		QString generate ();

};



#endif // AGENTNAMEGENERATOR_HPP
