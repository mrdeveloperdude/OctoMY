#ifndef AGENTMAIN_HPP
#define AGENTMAIN_HPP

#include "basic/GenericMain.hpp"

class Agent;
class AgentWindow;

class AgentMain: public GenericMain{
		Q_OBJECT
	private:

		Agent *agent;
		AgentWindow *window;

	public:
		explicit AgentMain(int argc, char *argv[]);

		virtual void start();
		virtual void stop();


};

int main(int argc, char *argv[]);



#endif // AGENTMAIN_HPP
