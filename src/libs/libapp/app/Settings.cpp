#include "app/Settings.hpp"
#include "utility/time/HumanTime.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "uptime/ConnectionType.hpp"

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QResource>
#include <QFile>



const qint64  Settings::MAX_SYNC_INTERVAL =		(1000 * 10); // Never sync settings to disk more often than once every 10 sec
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
	, mSettings(nullptr)
	, mLastSync(0)
{
	OC_METHODGATE();
	QCoreApplication::setOrganizationName(Settings::ORGANIZATION_NAME);
	QCoreApplication::setOrganizationDomain(Settings::DOMAIN_NAME);
	if(""!=appVersion) {
		QCoreApplication::setApplicationVersion(appVersion);
	}
	if(""!=appName) {
		QCoreApplication::setApplicationName(appName);
	}
	// Construct settings now that application details have been set

	mSettings=OC_NEW QSettings(this);
	if(nullptr!=mSettings) {
		//qDebug()<<"Created Settings for "<<group<<appName<<appVersion<<" in " << mSettings->fileName();
		if(""!=mGroup) {
			mSettings->beginGroup(mGroup);
		}
		syncTimer.setTimerType(Qt::VeryCoarseTimer);
		if(!connect(&syncTimer, SIGNAL(timeout()), this, SLOT(delayedSync()), OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect settings sync timer";
		}
	} else {
		qWarning()<<"ERROR: Could not create Settings for "<<group<<appName<<appVersion;

	}
}


Settings::~Settings()
{
	OC_METHODGATE();
	//qDebug()<<"DELETING Settings: "<<group<<appName<<appVersion;
	if(""!=mGroup) {
		mSettings->endGroup();
	}
	mSettings->deleteLater();
	mSettings=nullptr;
}





void Settings::resetConfiguration()
{
	OC_METHODGATE();
	if(nullptr!=mSettings)	{
		mSettings->clear();
	}
}


void Settings::sync()
{
	OC_METHODGATE();
	if(nullptr!=mSettings)	{
		const qint64 now=utility::time::currentMsecsSinceEpoch<qint64>();
		const qint64 timeSinceLastSync=now-mLastSync;
		if(timeSinceLastSync>MAX_SYNC_INTERVAL) {
			delayedSync();
		} else {
			syncTimer.start(static_cast<int>(MAX_SYNC_INTERVAL-timeSinceLastSync));
		}
	}
}

QString Settings::toString()
{
	QString allGroupsString;
	if(nullptr!=mSettings) {
		foreach (const QString &group, mSettings->childGroups()) {
			QString groupString = QString("==== %1\n").arg(group);
			mSettings->beginGroup(group);
			foreach (const QString &key, mSettings->childKeys()) {
				groupString.append(QString(" + %1 = %2\n").arg(key, mSettings->value(key).toString()));
			}
			mSettings->endGroup();
			allGroupsString.append(groupString);
		}
	} else {
		allGroupsString="null";
	}
	return allGroupsString;
}



void Settings::delayedSync()
{
	OC_METHODGATE();
	syncTimer.stop();
	if(nullptr!=mSettings)	{
		//qDebug()<<"SETTINGS SYNC PERFORMED TO "<<mSettings->fileName();
		mSettings->sync();
		mLastSync=utility::time::currentMsecsSinceEpoch<qint64>();
	}
}


///////////////////////////////////////




QByteArray Settings::getCustomSettingByteArray(const QString &sub, QByteArray def)
{
	OC_METHODGATE();
	if(nullptr==mSettings) {
		return def;
	}
	return mSettings->value(KEY_CUSTOM_SETTING_BASE+sub, def).toByteArray();
}

void Settings::setCustomSettingByteArray(const QString &sub, QByteArray val)
{
	OC_METHODGATE();
	if(nullptr!=mSettings) {
		QString s=(KEY_CUSTOM_SETTING_BASE+sub).trimmed();
		if(""!=s) {
			mSettings->setValue(s,val);
			//		qDebug()<<(KEY_CUSTOM_SETTING_BASE+sub)<<"="<<val;
			sync();
		} else {
			qWarning()<<"ERROR: Key was empty";
		}
	}
}




QString Settings::getCustomSetting(const QString &sub, QString def)
{
	OC_METHODGATE();
	if(nullptr==mSettings) {
		return def;
	}
	return mSettings->value(KEY_CUSTOM_SETTING_BASE+sub, def).toString();
}

void Settings::setCustomSetting(const QString &sub, QString val)
{
	OC_METHODGATE();
	if(nullptr!=mSettings) {
		QString s=(KEY_CUSTOM_SETTING_BASE+sub).trimmed();
		if(""!=s) {
			mSettings->setValue(s,val);
			//		qDebug()<<(KEY_CUSTOM_SETTING_BASE+sub)<<"="<<val;
			sync();
		} else {
			qWarning()<<"ERROR: Key was empty";
		}

	}
}


qint64  Settings::getCustomSettingLong(const QString &sub, qint64 def)
{
	OC_METHODGATE();
	if(nullptr==mSettings) {
		return def;
	}
	return mSettings->value(KEY_CUSTOM_SETTING_BASE+sub, def).toLongLong();
}

void Settings::setCustomSettingLong(const QString &sub,qint64  val)
{
	OC_METHODGATE();
	if(nullptr!=mSettings) {
		mSettings->setValue(KEY_CUSTOM_SETTING_BASE+sub,val);
		//		qDebug()<<(KEY_CUSTOM_SETTING_BASE+sub)<<"="<<val;
		sync();
	}
}


///


bool Settings::getCustomSettingBool(const QString &sub,bool def)
{
	OC_METHODGATE();
	if(nullptr==mSettings) {
		return def;
	}
	return mSettings->value(KEY_CUSTOM_SETTING_BASE+sub, def).toBool();

}

void Settings::setCustomSettingBool(const QString &sub,bool val)
{
	OC_METHODGATE();
	if(nullptr!=mSettings) {
		mSettings->setValue(KEY_CUSTOM_SETTING_BASE+sub,val);
		//		qDebug()<<(KEY_CUSTOM_SETTING_BASE+sub)<<"="<<val;
		sync();
	}
}



QRect Settings::getCustomSettingRect(const QString &sub, QRect def){
	OC_METHODGATE();
	if(nullptr==mSettings) {
		return def;
	}
	return mSettings->value(KEY_CUSTOM_SETTING_BASE+sub, def).toRect();
}


void Settings::setCustomSettingRect(const QString &sub, QRect val){
	if(nullptr!=mSettings) {
		mSettings->setValue(KEY_CUSTOM_SETTING_BASE+sub,val);
		//		qDebug()<<(KEY_CUSTOM_SETTING_BASE+sub)<<"="<<val;
		sync();
	}
}


bool Settings::hasCustomSetting(const QString &sub)
{
	OC_METHODGATE();
	if(nullptr==mSettings) {
		return false;
	}
	const QString bad="THIS_SETTING_WILL_NEVER_OCCUR_NATURALLY";
	return bad!=mSettings->value(KEY_CUSTOM_SETTING_BASE+sub, bad).toString();
}
