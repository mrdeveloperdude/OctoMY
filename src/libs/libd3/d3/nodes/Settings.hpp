#ifndef SETTINGS_H
#define SETTINGS_H

#include "uptime/SharedPointerWrapper.hpp"
#include <QSettings>

class QSettings;

class Settings
{
private:
	static QSharedPointer<QSettings> mSettings;
public:
	static QSharedPointer<QSettings> loadSettings(bool debug=false);
};

#endif // SETTINGS_H
