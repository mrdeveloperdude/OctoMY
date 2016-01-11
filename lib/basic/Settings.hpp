#ifndef SETTINGS_HPP
#define SETTINGS_HPP
#include <QSettings>
#include <QString>
#include <QFile>
#include <QTimer>

#define WW_QML_PREFIX "qrc:/qml/"

class Settings:QObject{
		Q_OBJECT
	private:
		QSettings *settings;
		static Settings *instance;

		QFile *versionFile;
		qint64 lastSync;
		QTimer syncTimer;

	public:
		static const qint64 MAX_SYNC_INTERVAL;

		static const QString CASUAL_COMPANY_NAME;
		static const QString CASUAL_DOMAIN_NAME;
		static const QString CASUAL_BRAND_NAME;
		static const QString CASUAL_APPLICATION_NAME;
		static const QString HOTLINE;
		static const QString USERAGENT;

		static const QString KEY_CUSTOM_SETTING_BASE;

	private:
		explicit Settings();
		virtual ~Settings();

		Q_DISABLE_COPY(Settings)
		//explicit Settings(Settings &a);
	public:

		QString getPlatformFingerprint();
		QString getCasualCompanyName();
		QString getCasualBrandName();
		QString getHotline();
		QString getUserAgent();

		QString getVersion();
		void setVersion(QString version);



		QString getCustomSetting(const QString &sub,QString def="");
		void setCustomSetting(const QString &sub,QString val);
		bool getCustomSettingBool(const QString &sub,bool def=false);
		qint64  getCustomSettingLong(const QString &sub, qint64 def=0);
		void setCustomSettingLong(const QString &sub,qint64  val);
		void setCustomSettingBool(const QString &sub,bool val);
		bool hasCustomSetting(const QString &sub);



	public:
		static Settings &getInstance();

		void resetConfiguration();

		void sync();

	private slots:
		void delayedSync();

};

#endif // SETTINGS_HPP
