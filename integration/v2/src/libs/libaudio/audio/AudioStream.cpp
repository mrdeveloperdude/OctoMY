#include "AudioStream.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QtCore>
#include <QAudioDeviceInfo>
#include <QAudioFormat>

AudioStream::AudioStream(IAudioSource &src, QObject *parent)
	: QIODevice(parent)
	, out(nullptr)
	, buf(nullptr)
	, bufsz(-1)
	, channelBytes(0)
	, sampleBytes(0)
	, secPerSample(1)
	, src(src)
	, okTriggered(false)
	, errorTriggered(false)
{
	OC_METHODGATE();
	qRegisterMetaType<QAudio::State>("QAudio::State");
}

void AudioStream::init()
{
	OC_METHODGATE();
	format.setSampleRate(44100);
	format.setChannelCount(1);
	format.setSampleSize(16);
	format.setCodec("audio/pcm");
	format.setByteOrder(QAudioFormat::LittleEndian);
	format.setSampleType(QAudioFormat::SignedInt);
	QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
	if (!info.isFormatSupported(format)) {
		qWarning() << "Default format not supported - trying to use nearest";
		format = info.nearestFormat(format);
	}
	qDebug()<<"Format used: "<<format;
	channelBytes = format.sampleSize() / 8;
	sampleBytes = format.channelCount() * channelBytes;
	secPerSample=1.0/static_cast<double>(format.sampleRate());
	out = OC_NEW QAudioOutput(format, static_cast<QObject *>(this));
	Q_ASSERT(nullptr!=out);
	connect(out, SIGNAL(notify()), this, SLOT(notified()));
	connect(out, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
	src.init(format);
	start();
	out->start(this);
	qDebug()<<"Started";
}

void AudioStream::start()
{
	OC_METHODGATE();
	open(QIODevice::ReadOnly);
}

void AudioStream::stop()
{
	OC_METHODGATE();
	close();
}

qint64 AudioStream::readData(char *data, const qint64 len)
{
	OC_METHODGATE();
	const qint64 samples=len/sampleBytes;
	if(samples>bufsz) {
		bufsz=samples;
		delete[] buf;
		buf=OC_NEW double[static_cast<size_t>(bufsz)];
	}
	src.generate(samples,buf);
	convertBuffer(samples,buf,data);
	return len;
}

qint64 AudioStream::writeData(const char *data, qint64 len)
{
	OC_METHODGATE();
	Q_UNUSED(data);
	Q_UNUSED(len);
	return 0;
}

qint64 AudioStream::bytesAvailable() const
{
	OC_METHODGATE();
	return bufsz;
}
