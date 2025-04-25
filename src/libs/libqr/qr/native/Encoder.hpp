#ifndef ENCODER_HPP
#define ENCODER_HPP

#include <QString>
#include <QVector>
#include <QImage>
#include <QBitArray>
#include "uptime/SharedPointerWrapper.hpp"

#include "qr/IQrGenerator.hpp"

class Code;
class Segment;
struct Ecc;

/**
 * @brief The Encoder class is the factory class for Code objects. It collects
 * data and options to produce a QR code into a new instance of Code
 */
class Encoder: public IQrGenerator{
private:
	bool mHasBegun{false};
	bool mHasEnded{false};
	QString mErrorString;
	QVector<QSharedPointer<Segment> > mSegments;
	int minVersion{1};
	int maxVersion{40};
	int mask{-1};
	Ecc *ecl{nullptr};

public:
	Encoder();
	
private:
	bool fail(const QString &reason);
	bool calculateVersion(int &version, int &dataUsedBits);

public:
	bool begin();
	bool appendSegment(QSharedPointer<Segment> segment);
	bool appendBytes(const QByteArray &bytes);
	bool appendNumeric(const QString &numbers);
	bool appendAlphaNumeric(const QString &alphaNumeric);
	bool appendAutodetect(const QString &data);
	
	
	bool setErrorCorrectionLevel(Ecc *level);
	
	bool setVersionSpan(int minVersion=1, int maxVersion=40);
	bool setMask(int mask);
	QSharedPointer<Code> end();

	QString errorString();

	// IQrGenerator interface
public:
	void append(const QString &text) override;
	QSharedPointer<IQrCode> generate() override;

};


extern QBitArray bytesToBits(const QByteArray &bytes);


#endif // ENCODER_HPP

