#ifndef MEDIADEVICEMANAGER_HPP
#define MEDIADEVICEMANAGER_HPP

#include <QObject>
#include <QList>
#include <QMediaDevices>

class MediaDevice; // Forward declaration

class MediaDeviceManager : public QObject {
	Q_OBJECT
private:
	// Internal list of media devices.
	QList<MediaDevice*> m_devices;
	QMediaDevices* m_mediaDevices;

public:
	explicit MediaDeviceManager(QObject* parent = nullptr);
	~MediaDeviceManager();
	
	// Returns the list of logical media devices (ownership remains here)
	QList<MediaDevice*> devices() const;
	
signals:
	void devicesChanged();
	
private slots:
	void onAudioInputsChanged();
	void onVideoInputsChanged();
	
private:
	void rebuildDeviceList();
	void clearDevices();
	// Heuristic: returns true if two device descriptions are similar.
	bool descriptionsMatch(const QString &desc1, const QString &desc2) const;
	
};

#endif // MEDIADEVICEMANAGER_HPP
