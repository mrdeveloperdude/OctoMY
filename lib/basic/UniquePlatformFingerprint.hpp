#ifndef UNIQUEPLATFORMFINGERPRINT_HPP
#define UNIQUEPLATFORMFINGERPRINT_HPP

#include <QString>
#include <QByteArray>

class UniquePlatformFingerprint{
	private:
		QByteArray raw;
		QString id;
		QString b32ww;
		qint64 qint;
		quint64 quint;
	private:
		UniquePlatformFingerprint();

		static UniquePlatformFingerprint *instance;
	public:
		static  UniquePlatformFingerprint & getInstance();
		QString getHEX();
		QString getBase32ww();
		QByteArray getRaw();
		quint64 getQuint64();
		qint64 getQint64();
		bool isValid();
};

#endif // UNIQUEPLATFORMFINGERPRINT_HPP
