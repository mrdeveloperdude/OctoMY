#include "AudioStream.hpp"

#include <QtCore>
#include <QAudioDeviceInfo>
#include <QAudioFormat>

AudioStream::AudioStream(IAudioSource &src, QObject *parent)
	: QIODevice(parent)
  , out(0)
  , buf(0)
  , bufsz(-1)
  , channelBytes(0)
  , sampleBytes(0)
  , secPerSample(1)
  , src(src)
  , okTriggered(false)
  , errorTriggered(false)
{
	qRegisterMetaType<QAudio::State>("QAudio::State");
}

void AudioStream::init(){
	srand(time(0));
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
	secPerSample=1.0/(double)format.sampleRate();
	out = new QAudioOutput(format, (QObject *)this);
	Q_ASSERT(0!=out);
	connect(out, SIGNAL(notify()), this, SLOT(notified()));
	connect(out, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
	src.init(format);
	start();
	out->start(this);
	qDebug()<<"Started";
}

void AudioStream::start(){
	open(QIODevice::ReadOnly);
}

void AudioStream::stop(){
	close();
}

qint64 AudioStream::readData(char *data, const qint64 len){
	const qint64 samples=len/sampleBytes;
	if(samples>bufsz){
		bufsz=samples;
		delete[] buf;
		buf=new double[bufsz];
	}
	src.generate(samples,buf);
	convertBuffer(samples,buf,data);
	return len;
}

qint64 AudioStream::writeData(const char *data, qint64 len){
	Q_UNUSED(data);
	Q_UNUSED(len);
	return 0;
}

qint64 AudioStream::bytesAvailable() const{
	return bufsz;
}
