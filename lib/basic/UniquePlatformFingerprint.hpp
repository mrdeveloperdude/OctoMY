#ifndef UNIQUEPLATFORMFINGERPRINT_HPP
#define UNIQUEPLATFORMFINGERPRINT_HPP

#include <QString>
#include <QByteArray>

class UniquePlatformFingerprint{
	private:
		QByteArray raw;
		QString id;
		QString b32ww;
	private:
		UniquePlatformFingerprint();

		static UniquePlatformFingerprint *instance;
	public:
		static  UniquePlatformFingerprint & getInstance();
		QString getHEX();
		QString getBase32ww();
		QByteArray getRaw();
		bool isValid();
};

#endif // UNIQUEPLATFORMFINGERPRINT_HPP
