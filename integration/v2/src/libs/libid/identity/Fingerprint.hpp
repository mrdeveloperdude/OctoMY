#ifndef FINGERPRINT_HPP
#define FINGERPRINT_HPP


#include <QString>
#include <QByteArray>


class Fingerprint{
	public:
		explicit Fingerprint(const QString raw="");
	private:
		QString in;
		QByteArray raw;
		QString id;
		QString b32ww;
		qint64 qint;
		quint64 quint;
	public:
		const QString getHEX() const;
		const QString getBase32ww() const;
		const QByteArray getRaw() const;
		quint64 getQuint64() const;
		qint64 getQint64() const;
		quint32 getQuint32() const;
		qint32 getQint32() const;
		bool isValid() const;
		QString toString();
};

#endif // FINGERPRINT_HPP
