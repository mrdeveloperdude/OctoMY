#ifndef APPRENDERINGSETTINGSPROVIDER_HPP
#define APPRENDERINGSETTINGSPROVIDER_HPP

#include <QSurfaceFormat>

class AppRenderingSettingsProvider
{
public:
	QSurfaceFormat surfaceFormat();
	Qt::ApplicationAttribute applicationAttributes();
};


#endif // APPRENDERINGSETTINGSPROVIDER_HPP
