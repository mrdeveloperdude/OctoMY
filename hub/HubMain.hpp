#ifndef SERVERMAIN_HPP
#define SERVERMAIN_HPP


#include "basic/GenericMain.hpp"

class Hub;
class HubWindow;

class HubMain: public GenericMain{
		Q_OBJECT
	private:

		Hub *hub;
		HubWindow *window;

	public:
		explicit HubMain(int argc, char *argv[]);

		virtual void start();
		virtual void stop();


};



int main(int argc, char *argv[]);


#endif // SERVERMAIN_HPP
