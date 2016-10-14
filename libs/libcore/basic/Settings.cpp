#include "basic/Settings.hpp"
#include <QCoreApplication>
#include "../libutil/utility/Standard.hpp"
#include "basic/UniquePlatformFingerprint.hpp"

#include <QDateTime>
#include <QDebug>
#include <QResource>
#include <QFile>



const qint64  Settings::MAX_SYNC_INTERVAL =		(1000 * 10); //Never sync more often than once every 10 sec
const QString Settings::ORGANIZATION_NAME =		"OctoMY™";
const QString Settings::DOMAIN_NAME =			"octomy.org™";
const QString Settings::BRAND_NAME =			"OctoMY™";
const QString Settings::APPLICATION_NAME_BASE =	BRAND_NAME;
const QString Settings::APPLICATION_VERSION =	"1.0";
const QString Settings::USERAGENT =				"OctoMY/"+APPLICATION_VERSION;


const QString Settings::KEY_CUSTOM_SETTING_BASE="";  // SET TO EMPTY STRING TO ALLOW SMOOTH TRANSITION FROM CUSTOM TO ACTUAL SETTING IN LATER VERSIONS "custom-setting-";


Settings::Settings(QString group, QString appName, QString appVersion, QObject *parent)
	: QObject(parent)
	, mGroup(group.trimmed())
	, mSettings(new QSettings)
	, mLastSync(0)
{
	OC_METHODGATE();
	QCoreApplication::setOrganizationName(Settings::ORGANIZATION_NAME);
	QCoreApplication::setOrganizationDomain(Settings::DOMAIN_NAME);
	if(""!=appVersion){
		QCoreApplication::setApplicationVersion(appVersion);
	}
	if(""!=appName){
		QCoreApplication::setApplicationName(appName);
	}
	if(""!=mGroup){
		mSettings->beginGroup(mGroup);
	}
	syncTimer.setTimerType(Qt::VeryCoarseTimer);
	if(!connect(&syncTimer, SIGNAL(timeout()), this, SLOT(delayedSync()), OC_CONTYPE)){
		qWarning()<<"ERROR: Could not connect settings sync timer";
	}
	//qDebug()<<"CREATING Settings: "<<group<<appName<<appVersion;
}


Settings::~Settings(){
	OC_METHODGATE();
	//qDebug()<<"DELETING Settings: "<<group<<appName<<appVersion;
	if(""!=mGroup){
		mSettings->endGroup();
	}
	delete mSettings;
	mSettings=nullptr;
}





void Settings::resetConfiguration(){
	OC_METHODGATE();
	if(nullptr!=mSettings)	{
		mSettings->clear();
	}
}


void Settings::sync(){
	OC_METHODGATE();
	if(nullptr!=mSettings)	{
		const qint64 now=QDateTime::currentMSecsSinceEpoch();
		const qint64 timeSinceLastSync=now-mLastSync;
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
	if(nullptr!=mSettings)	{
		//qDebug()<<"SETTINGS SYNC PERFORMED";
		mSettings->sync();
		mLastSync=QDateTime::currentMSecsSinceEpoch();
	}
}


///////////////////////////////////////




QByteArray Settings::getCustomSettingByteArray(const QString &sub, QByteArray def){
	OC_METHODGATE();
	if(0==mSettings){
		return def;
	}
	return mSettings->value(KEY_CUSTOM_SETTING_BASE+sub, def).toByteArray();
}

void Settings::setCustomSettingByteArray(const QString &sub, QByteArray val){
	OC_METHODGATE();
	if(nullptr!=mSettings){
		QString s=(KEY_CUSTOM_SETTING_BASE+sub).trimmed();
		if(""!=s){
			mSettings->setValue(s,val);
			//		qDebug()<<(KEY_CUSTOM_SETTING_BASE+sub)<<"="<<val;
			sync();
		}
		else{
			qWarning()<<"ERROR: Key was empty";
		}
	}
}




QString Settings::getCustomSetting(const QString &sub, QString def){
	OC_METHODGATE();
	if(0==mSettings){
		return def;
	}
	return mSettings->value(KEY_CUSTOM_SETTING_BASE+sub, def).toString();
}

void Settings::setCustomSetting(const QString &sub, QString val){
	OC_METHODGATE();
	if(nullptr!=mSettings){
		QString s=(KEY_CUSTOM_SETTING_BASE+sub).trimmed();
		if(""!=s){
			mSettings->setValue(s,val);
			//		qDebug()<<(KEY_CUSTOM_SETTING_BASE+sub)<<"="<<val;
			sync();
		}
		else{
			qWarning()<<"ERROR: Key was empty";
		}

	}
}


qint64  Settings::getCustomSettingLong(const QString &sub, qint64 def){
	OC_METHODGATE();
	if(nullptr==mSettings){
		return def;
	}
	return mSettings->value(KEY_CUSTOM_SETTING_BASE+sub, def).toLongLong();
}

void Settings::setCustomSettingLong(const QString &sub,qint64  val){
	OC_METHODGATE();
	if(nullptr!=mSettings){
		mSettings->setValue(KEY_CUSTOM_SETTING_BASE+sub,val);
		//		qDebug()<<(KEY_CUSTOM_SETTING_BASE+sub)<<"="<<val;
		sync();
	}
}


///


bool Settings::getCustomSettingBool(const QString &sub,bool def){
	OC_METHODGATE();
	if(nullptr==mSettings){
		return def;
	}
	return mSettings->value(KEY_CUSTOM_SETTING_BASE+sub, def).toBool();

}

void Settings::setCustomSettingBool(const QString &sub,bool val){
	OC_METHODGATE();
	if(nullptr!=mSettings){
		mSettings->setValue(KEY_CUSTOM_SETTING_BASE+sub,val);
		//		qDebug()<<(KEY_CUSTOM_SETTING_BASE+sub)<<"="<<val;
		sync();
	}
}

bool Settings::hasCustomSetting(const QString &sub){
	OC_METHODGATE();
	if(nullptr==mSettings){
		return false;
	}
	const QString bad="THIS_SETTING_WILL_NEVER_OCCUR_NATURALLY";
	return bad!=mSettings->value(KEY_CUSTOM_SETTING_BASE+sub, bad).toString();
}

///
