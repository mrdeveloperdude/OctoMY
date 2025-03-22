#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <QByteArray>
#include <QFile>
#include <QRect>
#include <QSettings>
#include <QString>
#include <QTimer>



class Settings: public QObject
{
	Q_OBJECT
private:
	QString mGroup;
	QSettings *mSettings;
	qint64 mLastSync;
	QTimer syncTimer;

public:
	static const qint64 MAX_SYNC_INTERVAL;

	static const QString ORGANIZATION_NAME;
	static const QString DOMAIN_NAME;
	static const QString BRAND_NAME;
	static const QString APPLICATION_NAME_BASE;
	static const QString APPLICATION_VERSION;
	static const QString USERAGENT;

	static const QString KEY_CUSTOM_SETTING_BASE;

public:
	explicit Settings(QString mGroup="", QString appName="", QString appVersion="", QObject *parent=nullptr);
	virtual ~Settings();

public:
	void resetConfiguration();
	void sync();
	QString toString();

public:
	QByteArray getCustomSettingByteArray(const QString &sub, QByteArray def=QByteArray());
	void setCustomSettingByteArray(const QString &sub, QByteArray val);


	QString getCustomSetting(const QString &sub,QString def="");
	void setCustomSetting(const QString &sub,QString val);
	bool getCustomSettingBool(const QString &sub,bool def=false);
	qint64  getCustomSettingLong(const QString &sub, qint64 def=0);
	void setCustomSettingLong(const QString &sub,qint64  val);
	void setCustomSettingBool(const QString &sub,bool val);
	QRect getCustomSettingRect(const QString &sub, QRect def=QRect());
	void setCustomSettingRect(const QString &sub, QRect val);
	bool hasCustomSetting(const QString &sub);

private slots:
	void delayedSync();

};

#endif // SETTINGS_HPP
