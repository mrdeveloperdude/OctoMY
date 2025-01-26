#include "Settings.hpp"

#include <QCoreApplication>

#include <QDebug>



QSharedPointer<QSettings> Settings::mSettings{nullptr};

QSharedPointer<QSettings> Settings::loadSettings(bool debug){
	if(mSettings.isNull()){
		QCoreApplication::setOrganizationName("OctoMY");
		QCoreApplication::setOrganizationDomain("octomy.org");
		QCoreApplication::setApplicationName("NodeView");
		const QString config_file = QString("%1.ini").arg(QCoreApplication::applicationName());
		Settings::mSettings = QSharedPointer<QSettings> (new QSettings(config_file, QSettings::IniFormat));
		if(debug){
			qDebug()<<"Loaded settings from:"<<Settings::mSettings->fileName();
		}
	}
	return Settings::mSettings;
}
