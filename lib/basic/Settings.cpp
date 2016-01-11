#include "basic/Settings.hpp"
#include <QCoreApplication>
#include "basic/Standard.hpp"
#include "basic/UniquePlatformFingerprint.hpp"

#include <QDateTime>
#include <QDebug>
#include <QResource>
#include <QFile>


Settings *Settings::instance=0;

const qint64  Settings::MAX_SYNC_INTERVAL=(1000 * 10); //Never sync more often than once every 10 sec
const QString Settings::CASUAL_COMPANY_NAME = "Playmio.com™";
const QString Settings::CASUAL_DOMAIN_NAME = "playmio.com™";
const QString Settings::CASUAL_BRAND_NAME = "OctoMY™";
const QString Settings::CASUAL_APPLICATION_NAME = "OctoMY™";
const QString Settings::HOTLINE = "+47 97688353";
const QString Settings::USERAGENT= "OctoMY/1.0";


const QString Settings::KEY_CUSTOM_SETTING_BASE="";  // SET TO EMPTY STRING TO ALLOW SMOOTH TRANSITION FROM CUSTOM TO ACTUAL SETTING IN LATER VERSIONS "custom-setting-";


Settings::Settings():
	QObject(0)
  , settings(0)
  , versionFile(0)
  , lastSync(0)
{
	WWMETHODGATE();
	QCoreApplication::setApplicationVersion("1.0");
	QCoreApplication::setOrganizationName(CASUAL_COMPANY_NAME);
	QCoreApplication::setOrganizationDomain(CASUAL_DOMAIN_NAME);
	QCoreApplication::setApplicationName(CASUAL_APPLICATION_NAME);
	settings=new QSettings;
	syncTimer.setTimerType(Qt::VeryCoarseTimer);
	if(!connect(&syncTimer,SIGNAL(timeout()),this,SLOT(delayedSync()),WWCONTYPE)){
		qWarning()<<"ERROR: Could not connect settings sync timer";
	}
}


Settings::~Settings(){
	WWMETHODGATE();
	//DELETE 2014-09-26
	delete settings;
	settings=0;
}

/*
Settings::Settings(Settings &){
	WWMETHODGATE();
	//This is simply an inert placeholder
}
*/



//TODO: REPLACE SINGLETON
Settings &Settings::getInstance(){
	if(0==instance){
		instance=new Settings;
	}
	if(0==instance){
		qWarning() << "ERROR: Settings object null!";
	}
	return *instance;
}


void Settings::resetConfiguration(){
	WWMETHODGATE();
}


void Settings::sync(){
	WWMETHODGATE();
	if(0!=settings)	{
		const qint64 now=QDateTime::currentMSecsSinceEpoch();
		const qint64 timeSinceLastSync=now-lastSync;
		if(timeSinceLastSync>MAX_SYNC_INTERVAL){
			delayedSync();
		}
		else{
			syncTimer.start(MAX_SYNC_INTERVAL-timeSinceLastSync);
		}
	}
}




void Settings::delayedSync(){
	WWMETHODGATE();
	syncTimer.stop();
	if(0!=settings)	{
		qDebug()<<"SETTINGS SYNC PERFORMED";
		settings->sync();
		lastSync=QDateTime::currentMSecsSinceEpoch();
	}
}


///////////////////////////////////////




QString Settings::getCustomSetting(const QString &sub, QString def){
	WWMETHODGATE();
	if(0==settings){
		return def;
	}
	return settings->value(KEY_CUSTOM_SETTING_BASE+sub, def).toString();
}

void Settings::setCustomSetting(const QString &sub,QString val){
	WWMETHODGATE();
	if(0!=settings){
		settings->setValue(KEY_CUSTOM_SETTING_BASE+sub,val);
		//		qDebug()<<(KEY_CUSTOM_SETTING_BASE+sub)<<"="<<val;
		sync();
	}
}


qint64  Settings::getCustomSettingLong(const QString &sub, qint64 def){
	WWMETHODGATE();
	if(0==settings){
		return def;
	}
	return settings->value(KEY_CUSTOM_SETTING_BASE+sub, def).toLongLong();
}

void Settings::setCustomSettingLong(const QString &sub,qint64  val){
	WWMETHODGATE();
	if(0!=settings){
		settings->setValue(KEY_CUSTOM_SETTING_BASE+sub,val);
		//		qDebug()<<(KEY_CUSTOM_SETTING_BASE+sub)<<"="<<val;
		sync();
	}
}


///


bool Settings::getCustomSettingBool(const QString &sub,bool def){
	WWMETHODGATE();
	if(0==settings){
		return def;
	}
	return settings->value(KEY_CUSTOM_SETTING_BASE+sub, def).toBool();

}

void Settings::setCustomSettingBool(const QString &sub,bool val){
	WWMETHODGATE();
	if(0!=settings){
		settings->setValue(KEY_CUSTOM_SETTING_BASE+sub,val);
		//		qDebug()<<(KEY_CUSTOM_SETTING_BASE+sub)<<"="<<val;
		sync();
	}
}

bool Settings::hasCustomSetting(const QString &sub){
	WWMETHODGATE();
	if(0==settings){
		return false;
	}
	const QString bad="THIS_SETTING_WILL_NEVER_OCCUR_NATURALLY";
	return bad!=settings->value(KEY_CUSTOM_SETTING_BASE+sub, bad).toString();
}

///
