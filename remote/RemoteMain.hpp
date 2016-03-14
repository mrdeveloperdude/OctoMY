#ifndef REMOTEMAIN_HPP
#define REMOTEMAIN_HPP

#include "basic/GenericMain.hpp"

class Remote;
class RemoteWindow;

class RemoteMain: public GenericMain{
		Q_OBJECT
	private:

		Remote *remote;
		RemoteWindow *window;

	public:
		explicit RemoteMain(int argc, char *argv[]);

		virtual void start();
		virtual void stop();


};

int main(int argc, char *argv[]);



#endif // REMOTEMAIN_HPP
