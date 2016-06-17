#include "basic/Settings.hpp"
#include <QCoreApplication>
#include "basic/Standard.hpp"
#include "basic/UniquePlatformFingerprint.hpp"

#include <QDateTime>
#include <QDebug>
#include <QResource>
#include <QFile>



const qint64  Settings::MAX_SYNC_INTERVAL=(1000 * 10); //Never sync more often than once every 10 sec
const QString Settings::ORGANIZATION_NAME = "OctoMY™";
const QString Settings::DOMAIN_NAME = "octomy.org™";
const QString Settings::BRAND_NAME = "OctoMY™";
const QString Settings::APPLICATION_NAME = "OctoMY™";
const QString Settings::USERAGENT= "OctoMY/1.0";


const QString Settings::KEY_CUSTOM_SETTING_BASE="";  // SET TO EMPTY STRING TO ALLOW SMOOTH TRANSITION FROM CUSTOM TO ACTUAL SETTING IN LATER VERSIONS "custom-setting-";


Settings::Settings(QObject *parent):
	QObject(parent)
  , settings(nullptr)
  , lastSync(0)
{
	OC_METHODGATE();
	settings=new QSettings;
	syncTimer.setTimerType(Qt::VeryCoarseTimer);
	if(!connect(&syncTimer,SIGNAL(timeout()),this,SLOT(delayedSync()),OC_CONTYPE)){
		qWarning()<<"ERROR: Could not connect settings sync timer";
	}
}


Settings::~Settings(){
	OC_METHODGATE();
	//DELETE 2014-09-26
	delete settings;
	settings=0;
}





void Settings::resetConfiguration(){
	OC_METHODGATE();
	if(nullptr!=settings)	{
		settings->clear();
	}
}


void Settings::sync(){
	OC_METHODGATE();
	if(nullptr!=settings)	{
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
	OC_METHODGATE();
	syncTimer.stop();
	if(0!=settings)	{
		//qDebug()<<"SETTINGS SYNC PERFORMED";
		settings->sync();
		lastSync=QDateTime::currentMSecsSinceEpoch();
	}
}


///////////////////////////////////////




QString Settings::getCustomSetting(const QString &sub, QString def){
	OC_METHODGATE();
	if(0==settings){
		return def;
	}
	return settings->value(KEY_CUSTOM_SETTING_BASE+sub, def).toString();
}

void Settings::setCustomSetting(const QString &sub,QString val){
	OC_METHODGATE();
	if(0!=settings){
		settings->setValue(KEY_CUSTOM_SETTING_BASE+sub,val);
		//		qDebug()<<(KEY_CUSTOM_SETTING_BASE+sub)<<"="<<val;
		sync();
	}
}


qint64  Settings::getCustomSettingLong(const QString &sub, qint64 def){
	OC_METHODGATE();
	if(0==settings){
		return def;
	}
	return settings->value(KEY_CUSTOM_SETTING_BASE+sub, def).toLongLong();
}

void Settings::setCustomSettingLong(const QString &sub,qint64  val){
	OC_METHODGATE();
	if(0!=settings){
		settings->setValue(KEY_CUSTOM_SETTING_BASE+sub,val);
		//		qDebug()<<(KEY_CUSTOM_SETTING_BASE+sub)<<"="<<val;
		sync();
	}
}


///


bool Settings::getCustomSettingBool(const QString &sub,bool def){
	OC_METHODGATE();
	if(0==settings){
		return def;
	}
	return settings->value(KEY_CUSTOM_SETTING_BASE+sub, def).toBool();

}

void Settings::setCustomSettingBool(const QString &sub,bool val){
	OC_METHODGATE();
	if(0!=settings){
		settings->setValue(KEY_CUSTOM_SETTING_BASE+sub,val);
		//		qDebug()<<(KEY_CUSTOM_SETTING_BASE+sub)<<"="<<val;
		sync();
	}
}

bool Settings::hasCustomSetting(const QString &sub){
	OC_METHODGATE();
	if(0==settings){
		return false;
	}
	const QString bad="THIS_SETTING_WILL_NEVER_OCCUR_NATURALLY";
	return bad!=settings->value(KEY_CUSTOM_SETTING_BASE+sub, bad).toString();
}

///
