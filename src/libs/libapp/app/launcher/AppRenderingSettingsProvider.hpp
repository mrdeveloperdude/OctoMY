#ifndef APPRENDERINGSETTINGSPROVIDER_HPP
#define APPRENDERINGSETTINGSPROVIDER_HPP

#include <QSurfaceFormat>

#include <QMap>

class QApplication;

class AppRenderingSettingsProvider
{
public:
	QSurfaceFormat surfaceFormat();
	QMap<Qt::ApplicationAttribute, bool>  applicationAttributes();
	void applyApplicationAttributes();
};


#endif
// APPRENDERINGSETTINGSPROVIDER_HPP


