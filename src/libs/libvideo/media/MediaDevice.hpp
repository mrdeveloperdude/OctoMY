#ifndef MEDIADEVICE_HPP
#define MEDIADEVICE_HPP

#include <QObject>
#include <optional>
#include <QString>
#include <QAudioDevice>
#include <QCameraDevice>
#include <QPixmap>
#include <QTimer>

// Forward declarations for preview capture.
class QCamera;
class QMediaCaptureSession;
class QVideoSink;
class QAudioSource;
class QIODevice;
class QVideoFrame;

class MediaDevice : public QObject {
	Q_OBJECT
	Q_PROPERTY(QString name READ name CONSTANT)
	Q_PROPERTY(QString details READ details CONSTANT)
	Q_PROPERTY(bool hasAudio READ hasAudio CONSTANT)
	Q_PROPERTY(bool hasVideo READ hasVideo CONSTANT)
	Q_PROPERTY(bool previewActive READ previewActive WRITE setPreviewActive NOTIFY previewActiveChanged)
	Q_PROPERTY(QPixmap previewPixmap READ previewPixmap NOTIFY previewPixmapChanged)
	
	
private:
	// Data members.
	std::optional<QAudioDevice> m_audioDevice;
	std::optional<QCameraDevice> m_videoDevice;
	bool m_previewActive = false;
	QPixmap m_previewPixmap;
	
	// Preview capture for video.
	QCamera* m_camera = nullptr;
	QMediaCaptureSession* m_captureSession = nullptr;
	QVideoSink* m_videoSink = nullptr;
	
	// Preview capture for audio.
	QAudioSource* m_audioSource = nullptr;
	QIODevice* m_audioIODevice = nullptr;
	QTimer* m_audioTimer = nullptr;

public:
	explicit MediaDevice(QObject* parent = nullptr);
	~MediaDevice();
	
	void setAudioDevice(const QAudioDevice &audio);
	void setVideoDevice(const QCameraDevice &video);
	
	bool hasAudio() const;
	bool hasVideo() const;
	QString name() const;
	QString details() const;
	
	bool previewActive() const;
	void setPreviewActive(bool active);
	
	QPixmap previewPixmap() const;
	
	QAudioFormat audioFormat() const;
	QSize videoResolution() const;
	double videoFrameRate() const;
	
	
	
signals:
	void previewActiveChanged();
	void previewPixmapChanged();
	
private slots:
	// Slot for handling video frame updates.
	void onVideoFrameChanged(const QVideoFrame &frame);
	// Slot to simulate audio waveform update.
	void onAudioBufferReady();
	
private:
	// Helpers to start/stop preview capture.
	void startVideoPreview();
	void stopVideoPreview();
	void startAudioPreview();
	void stopAudioPreview();
};

#endif // MEDIADEVICE_HPP
