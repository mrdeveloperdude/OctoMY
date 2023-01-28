#ifndef AUDIOSTREAM_HPP
#define AUDIOSTREAM_HPP

#include "IAudioSource.hpp"

#include <QObject>
#include <QAudioOutput>
#include <QDebug>
#include <QtEndian>

class AudioStream: public QIODevice
{
	Q_OBJECT
private:

	QAudioFormat format;
	QAudioOutput *out;

	double *buf;
	qint64 bufsz;

	int channelBytes;
	int sampleBytes;
	double secPerSample;
	IAudioSource &src;
	bool okTriggered;
	bool errorTriggered;


public:
	explicit AudioStream(IAudioSource &src, QObject *parent=nullptr);
	virtual ~AudioStream() {}

public:
	void init();

	void ok()
	{
		okTriggered=true;
	}

	void error()
	{
		errorTriggered=true;
	}

private:

	inline void f64_i16s(int num, double *in,char *out)
	{
		uchar *ptr=(uchar *)out;
		if (format.byteOrder() == QAudioFormat::LittleEndian) {
			for(int i=0; i<num; ++i) {
				const double x=in[i];
				qint16 value = static_cast<qint16>(x * 32767);
				qToLittleEndian<qint16>(value, ptr);
				ptr+=sampleBytes;
			}
		} else {
			for(int i=0; i<num; ++i) {
				const double x=in[i];
				qint16 value = static_cast<qint16>(x * 32767);
				qToBigEndian<qint16>(value, ptr);
				ptr+=sampleBytes;
			}
		}
	}


	inline void f64_i16u(int num, double *in,char *out)
	{
		uchar *ptr=(uchar *)out;
		if (format.byteOrder() == QAudioFormat::LittleEndian) {
			for(int i=0; i<num; ++i) {
				const double x=in[i];
				quint16 value = static_cast<quint16>((1.0 + x) / 2 * 65535);
				qToLittleEndian<qint16>(value, ptr);
				ptr+=sampleBytes;
			}
		} else {
			for(int i=0; i<num; ++i) {
				const double x=in[i];
				quint16 value = static_cast<quint16>((1.0 + x) / 2 * 65535);
				qToBigEndian<qint16>(value, ptr);
				ptr+=sampleBytes;
			}
		}
	}


	inline void f64_i8s(int num, double *in, char *out)
	{
		for(int i=0; i<num; ++i) {
			const double x=in[i];
			const qint8 value = static_cast<qint8>(x * 127);
			*reinterpret_cast<quint8*>(out) = value;
		}
	}


	inline void f64_i8u(int num, double *in, char *out)
	{
		for(int i=0; i<num; ++i) {
			const double x=in[i];
			const quint8 value = static_cast<quint8>((1.0 + x) / 2 * 255);
			*reinterpret_cast<quint8*>(out) = value;
		}
	}

	inline void convertBuffer(qint64 num, double *in, char *out)
	{
		const int sampleSize=format.sampleSize();
		const QAudioFormat::SampleType sampleType=format.sampleType();
		if (8==sampleSize ) {
			if(QAudioFormat::SignedInt==sampleType) {
				f64_i8s(num, in,out);
			} else if(QAudioFormat::UnSignedInt==sampleType) {
				f64_i8u(num, in,out);
			} else {
				qWarning()<<"ERROR: unknown sampleType: "<<sampleType;
			}
		} else if(16==sampleSize) {
			if(QAudioFormat::SignedInt==sampleType) {
				f64_i16s(num, in,out);
			} else if(QAudioFormat::UnSignedInt==sampleType) {
				f64_i16u(num, in,out);
			} else {
				qWarning()<<"ERROR: unknown sampleType: "<<sampleType;
			}
		} else {
			qWarning()<<"ERROR: unknown sampleSize: "<<sampleSize;
		}
	}


public:
	void start();
	void stop();

	qint64 readData(char *data, qint64 maxlen);
	qint64 writeData(const char *data, qint64 len);
	qint64 bytesAvailable() const;

signals:

	void signalPrepareSampleBuffer(int num, double buf[]);

public slots:


	void notified()
	{
		//qWarning() << "bytesFree = " << out->bytesFree()					   << ", " << "elapsedUSecs = " << out->elapsedUSecs()					   << ", " << "processedUSecs = " << out->processedUSecs();
	}


	void handleStateChanged(QAudio::State)
	{
		//	qWarning() << "state = " << state;
	}

};


#endif // AUDIOSTREAM_HPP
