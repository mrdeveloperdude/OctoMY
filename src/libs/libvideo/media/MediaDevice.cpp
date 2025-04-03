#include "MediaDevice.hpp"

#include "TestImageGenerator.hpp"


#include <QAudioSource>
#include <QCamera>
#include <QCameraFormat>
#include <QDebug>
#include <QIODevice>
#include <QMediaCaptureSession>
#include <QPainter>
#include <QRandomGenerator>
#include <QTimer>
#include <QVideoFrame>
#include <QVideoSink>


MediaDevice::MediaDevice(QObject* parent)
	: QObject(parent)
	, m_camera(nullptr)
	, m_videoSink(nullptr)
	, m_audioSource(nullptr)
	, m_audioIODevice(nullptr)
	, m_audioTimer(nullptr)
{
}

MediaDevice::~MediaDevice()
{
	stopVideoPreview();
	stopAudioPreview();
}

void MediaDevice::setAudioDevice(const QAudioDevice &audio)
{
	m_audioDevice = audio;
}

void MediaDevice::setVideoDevice(const QCameraDevice &video)
{
	m_videoDevice = video;
}

bool MediaDevice::hasAudio() const
{
	return m_audioDevice.has_value();
}

bool MediaDevice::hasVideo() const
{
	return m_videoDevice.has_value();
}

QString MediaDevice::name() const
{
	// Prefer video device description if available, otherwise audio.
	if (hasVideo())
		return m_videoDevice->description();
	else if (hasAudio())
		return m_audioDevice->description();
	return QStringLiteral("Unknown Device");
}

QString MediaDevice::details() const
{
	QString detail;
	if (hasVideo()) {
		detail += QStringLiteral("Video: %1").arg(m_videoDevice->description());
	}
	if (hasAudio()) {
		if (!detail.isEmpty())
			detail += QStringLiteral(" | ");
		detail += QStringLiteral("Audio: %1").arg(m_audioDevice->description());
	}
	return detail;
}

bool MediaDevice::previewActive() const
{
	return m_previewActive;
}

void MediaDevice::setPreviewActive(bool active)
{
	if (m_previewActive == active)
		return;
	m_previewActive = active;
	emit previewActiveChanged();
	
	if (m_previewActive) {
		// Start capture for available streams.
		if (hasVideo())
			startVideoPreview();
		// If there is no video, use audio preview.
		if (hasAudio() && !hasVideo())
			startAudioPreview();
	} else {
		stopVideoPreview();
		stopAudioPreview();
	}
	emit previewPixmapChanged();
}

QPixmap MediaDevice::previewPixmap() const
{
	return m_previewPixmap;
}

void MediaDevice::startVideoPreview()
{
	if (!hasVideo() || m_camera){
		return;
	}
	// Create a QCamera from the video device.
	m_camera = new QCamera(*m_videoDevice, this);
	// Create a QVideoSink to receive video frames.
	m_videoSink = new QVideoSink(this);
	// Create a QMediaCaptureSession to link the camera and video sink.
	m_captureSession = new QMediaCaptureSession(this);
	m_captureSession->setCamera(m_camera);
	m_captureSession->setVideoOutput(m_videoSink);
	connect(m_videoSink, &QVideoSink::videoFrameChanged, this, &MediaDevice::onVideoFrameChanged);
	
	m_previewPixmap = TestImageGenerator::generateTestFrame({128, (128*9)/16});
	emit previewPixmapChanged();
	
	m_camera->start();
}


void MediaDevice::stopVideoPreview()
{
	if (m_camera) {
		m_camera->stop();
		delete m_camera;
		m_camera = nullptr;
	}
	else{
		qWarning() << "No camera for preview of " << name();
	}
	if (m_captureSession) {
		delete m_captureSession;
		m_captureSession = nullptr;
	}
	else{
		qWarning() << "No capture session for preview of " << name();
	}
	if (m_videoSink) {
		delete m_videoSink;
		m_videoSink = nullptr;
	}
	else{
		qWarning() << "No video sink for preview of " << name();
	}
	m_previewPixmap = QPixmap();
	emit previewPixmapChanged();
}


void MediaDevice::onVideoFrameChanged(const QVideoFrame &frame)
{
	//qDebug() << "Trying to process frame";
	if (!frame.isValid()){
		qWarning() << "Got invalid frame for" << name();
		return;
	}
	// Create a copy and explicitly map the frame for reading.
	QVideoFrame copy(frame);
	if (!copy.map(QVideoFrame::ReadOnly)) {
		qWarning() << "Failed to map video frame for preview";
		return;
	}
	// Convert to QImage. You may also need to check the frame format if necessary.
	QImage image = copy.toImage();
	copy.unmap();
	if (!image.isNull()) {
		// Scale the image to your desired preview size.
		m_previewPixmap = QPixmap::fromImage(image).scaled(128, 128, Qt::KeepAspectRatio, Qt::FastTransformation);
		emit previewPixmapChanged();
	}
	else{
		qWarning() << "Problem converting to image for" << name();
	}
}


void MediaDevice::startAudioPreview()
{
	if (!hasAudio() || m_audioTimer) {
		return;
	}
	
	// Retrieve the preferred audio format from the device.
	QAudioFormat preferredFormat = m_audioDevice->preferredFormat();
	
	// Configure our audio format using the device's preferred settings.
	QAudioFormat format;
	format.setSampleRate(preferredFormat.sampleRate());
	format.setChannelCount(preferredFormat.channelCount());
	// Set sample format to 16-bit signed PCM.
	format.setSampleFormat(QAudioFormat::Int16);
	// In Qt6, byte order is determined automatically by the sample format and platform.
	
	// Create and start the audio source.
	m_audioSource = new QAudioSource(*m_audioDevice, format, this);
	m_audioIODevice = m_audioSource->start();
	if (!m_audioIODevice) {
		qWarning() << "Failed to start audio source for" << name();
		return;
	}
	
	// Create a timer to periodically update the waveform preview.
	m_audioTimer = new QTimer(this);
	connect(m_audioTimer, &QTimer::timeout, this, &MediaDevice::onAudioBufferReady);
	m_audioTimer->start(100); // update every 100 ms.
}


void MediaDevice::stopAudioPreview()
{
	if (m_audioTimer) {
		m_audioTimer->stop();
		delete m_audioTimer;
		m_audioTimer = nullptr;
	}
	if (m_audioSource) {
		m_audioSource->stop();
		delete m_audioSource;
		m_audioSource = nullptr;
	}
	m_audioIODevice = nullptr;
	m_previewPixmap = QPixmap();
	emit previewPixmapChanged();
}


void MediaDevice::onAudioBufferReady()
{
	// Ensure we have a valid audio stream.
	if (!m_audioIODevice || !m_audioSource){
		qWarning() << "No audio io device or source";
		return;
	}
	
	// Read all available audio data.
	QByteArray audioData = m_audioIODevice->readAll();
	if (audioData.isEmpty()){
		qWarning() << "No audio data";
		return;
	}
	
	// Get audio format details. We assume 16-bit signed PCM.
	QAudioFormat format = m_audioSource->format();
	int channelCount = format.channelCount();
	// Calculate sample size in bits from bytes per sample.
	int sampleSize = format.bytesPerSample() * 8;
	int sampleBytes = format.bytesPerSample();
	if (sampleBytes <= 0 || channelCount <= 0){
		qWarning() << "sample bytes or channel count < 0";
		return;
	}
	
	// Calculate total samples and samples per channel.
	int totalSamples = audioData.size() / sampleBytes;
	int samplesPerChannel = totalSamples / channelCount;
	if (samplesPerChannel <= 0){
		qWarning() << "samples per channel < 0";
		return;
	}
	
	// Prepare per-channel storage for normalized samples (range: -1.0 .. 1.0).
	QVector<QVector<qreal>> channels;
	channels.resize(channelCount);
	for (int ch = 0; ch < channelCount; ch++) {
		channels[ch].resize(samplesPerChannel);
	}
	
	// Process interleaved audio data.
	const char* data = audioData.constData();
	for (int i = 0; i < samplesPerChannel; i++) {
		for (int ch = 0; ch < channelCount; ch++) {
			int index = (i * channelCount + ch) * sampleBytes;
			if (index + sampleBytes > audioData.size())
				break;
			// Assuming 16-bit PCM.
			qint16 sample = *reinterpret_cast<const qint16*>(data + index);
			// Normalize sample to range -1.0 .. 1.0.
			qreal normalized = sample / 32768.0;
			channels[ch][i] = normalized;
		}
	}
	
	// Create a pixmap for the waveform preview.
	QPixmap pixmap(128, 128);
	pixmap.fill(Qt::black);
	QPainter painter(&pixmap);
	
	// Determine color for each channel.
	// Start at hue 120 (green) and step by 360/channelCount degrees.
	const int baseHue = 120;
	double stepHue = channelCount > 0 ? 360.0 / channelCount : 0;
	
	// Draw one waveform per channel.
	for (int ch = 0; ch < channelCount; ch++) {
		int hue = (baseHue + int(ch * stepHue)) % 360;
		QColor color;
		color.setHsl(hue, 255, 128); // Full saturation, medium lightness.
		painter.setPen(color);
		
		// Calculate vertical center for this channel.
		int yCenter = pixmap.height() * (ch + 1) / (channelCount + 1);
		
		// Create a polyline using the samples.
		QPolygon polyline;
		polyline.resize(samplesPerChannel);
		for (int i = 0; i < samplesPerChannel; i++) {
			int x = (i * pixmap.width()) / samplesPerChannel;
			// Scale amplitude to 1/4 of the pixmap height.
			int yOffset = int(channels[ch][i] * (pixmap.height() / 4));
			int y = yCenter - yOffset;
			polyline[i] = QPoint(x, y);
		}
		painter.drawPolyline(polyline);
	}
	painter.end();
	
	// Update the preview pixmap and notify the model/view.
	m_previewPixmap = pixmap;
	emit previewPixmapChanged();
}




QAudioFormat MediaDevice::audioFormat() const {
	return m_audioDevice.has_value() ? m_audioDevice->preferredFormat() : QAudioFormat();
}

QSize MediaDevice::videoResolution() const {
	if (m_videoDevice.has_value()) {
		auto formats = m_videoDevice->videoFormats();
		if (!formats.isEmpty()){
			return formats.first().resolution(); // Pick the first format or choose a preferred one.
		}
	}
	return QSize();
}

double MediaDevice::videoFrameRate() const {
	if (m_videoDevice.has_value()) {
		auto formats = m_videoDevice->videoFormats();
		if (!formats.isEmpty()){
			return formats.first().maxFrameRate(); // Pick the first format or a computed one.
		}
	}
	return 0.0;
}
