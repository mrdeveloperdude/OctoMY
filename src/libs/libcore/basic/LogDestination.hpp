#ifndef LOGDESTINATION_HPP
#define LOGDESTINATION_HPP

#include <QString>

class LogDestination{
	public:
		LogDestination();

		virtual void appendLog(const QString& text)=0;

};

#endif // LOGDESTINATION_HPP
