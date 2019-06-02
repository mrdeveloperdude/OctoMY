#ifndef WEBSITE_HPP
#define WEBSITE_HPP

#include "app/AppContext.hpp"

#include <QDir>

class Website
{
private:
	AppContext *mAppContext;
	QStringList mSources;
public:
	Website( AppContext *appContext);

public:

	void run();

private:

	void updateSourcesList(QDir dir);
};


#endif // WEBSITE_HPP
