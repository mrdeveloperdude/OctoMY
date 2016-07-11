#ifndef SETTINGS_HPP
#define SETTINGS_HPP
#include <QSettings>
#include <QString>
#include <QFile>
#include <QTimer>
#include <QByteArray>

class Settings:QObject{
		Q_OBJECT
	private:
		QString group;
		QSettings *settings;
		qint64 lastSync;
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
		explicit Settings(QString group="", QString appName="", QString appVersion="", QObject *parent=nullptr);
		virtual ~Settings();

	public:

		QByteArray getCustomSettingByteArray(const QString &sub, QByteArray def=QByteArray());
		void setCustomSettingByteArray(const QString &sub, QByteArray val);


		QString getCustomSetting(const QString &sub,QString def="");
		void setCustomSetting(const QString &sub,QString val);
		bool getCustomSettingBool(const QString &sub,bool def=false);
		qint64  getCustomSettingLong(const QString &sub, qint64 def=0);
		void setCustomSettingLong(const QString &sub,qint64  val);
		void setCustomSettingBool(const QString &sub,bool val);
		bool hasCustomSetting(const QString &sub);



	public:

		void resetConfiguration();

		void sync();

	private slots:
		void delayedSync();

};

#endif // SETTINGS_HPP
