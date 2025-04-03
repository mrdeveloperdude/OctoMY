#include "MediaDeviceManager.hpp"
#include "MediaDevice.hpp"
#include <QDebug>
#include <QAudioDevice>
#include <QCameraDevice>
#include <QRegularExpression>
#include <algorithm>
#include <vector>

// Helper functions for fuzzy string matching.
namespace {
// Compute the Levenshtein distance between two strings.
int levenshteinDistance(const QString &s1, const QString &s2)
{
	int len1 = s1.length(), len2 = s2.length();
	std::vector<std::vector<int>> d(len1 + 1, std::vector<int>(len2 + 1));
	for (int i = 0; i <= len1; ++i){
		d[i][0] = i;
	}
	for (int j = 0; j <= len2; ++j){
		d[0][j] = j;
	}
	for (int i = 1; i <= len1; ++i) {
		for (int j = 1; j <= len2; ++j) {
			int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
			d[i][j] = std::min({ d[i - 1][j] + 1, d[i][j - 1] + 1, d[i - 1][j - 1] + cost });
		}
	}
	return d[len1][len2];
}

// Returns a normalized similarity (1.0 means exact match).
double similarity(const QString &s1, const QString &s2)
{
	if (s1.isEmpty() && s2.isEmpty()) {
		return 1.0;
	}
	if (s1.isEmpty() || s2.isEmpty()) {
		return 0.0;
	}
	int distance = levenshteinDistance(s1, s2);
	int maxLen = std::max(s1.length(), s2.length());
	return 1.0 - double(distance) / double(maxLen);
}
}

MediaDeviceManager::MediaDeviceManager(QObject* parent)
	: QObject(parent)
	, m_mediaDevices(new QMediaDevices(this))
{
	// Connect to QMediaDevices signals for audio and video changes.
	connect(m_mediaDevices, &QMediaDevices::audioInputsChanged, this, &MediaDeviceManager::onAudioInputsChanged);
	connect(m_mediaDevices, &QMediaDevices::videoInputsChanged, this, &MediaDeviceManager::onVideoInputsChanged);
	
	// Initial build of the device list.
	rebuildDeviceList();
}

MediaDeviceManager::~MediaDeviceManager()
{
	clearDevices();
}

QList<MediaDevice*> MediaDeviceManager::devices() const
{
	return m_devices;
}

void MediaDeviceManager::onAudioInputsChanged()
{
	rebuildDeviceList();
	emit devicesChanged();
}

void MediaDeviceManager::onVideoInputsChanged()
{
	rebuildDeviceList();
	emit devicesChanged();
}

void MediaDeviceManager::clearDevices()
{
	// Delete all MediaDevice objects.
	qDeleteAll(m_devices);
	m_devices.clear();
}

bool MediaDeviceManager::descriptionsMatch(const QString &desc1, const QString &desc2) const
{
	static const QRegularExpression re("\\(([0-9a-f]{4}:[0-9a-f]{4})\\)");
	// Lowercase the descriptions for case-insensitive matching.
	QString s1 = desc1.toLower();
	QString s2 = desc2.toLower();
	
	// First, attempt to extract USB ID patterns (e.g. "(046d:0825)").
	
	auto match1 = re.match(s1);
	auto match2 = re.match(s2);
	if (match1.hasMatch() && match2.hasMatch()) {
		if (match1.captured(1) == match2.captured(1)){
			return true;
		}
	}
	
	// If no USB IDs are found or they don't match, use fuzzy matching.
	double sim = similarity(s1, s2);
	// Adjust the threshold based on testing; here, 0.5 is used.
	if (sim >= 0.5){
		return true;
	}

	return false;
}

void MediaDeviceManager::rebuildDeviceList()
{
	for (auto &device : m_devices) {
		delete device;
	}
	m_devices.clear();
	auto audioList = m_mediaDevices->audioInputs();
	auto videoList = m_mediaDevices->videoInputs();
	QList<MediaDevice*> newDevices;
	for (const auto &video : std::as_const(videoList))
	{
		MediaDevice* device = new MediaDevice(this);
		device->setVideoDevice(video);
		for (const QAudioDevice &audio : std::as_const(audioList))
		{
			if (descriptionsMatch(video.description(), audio.description()))
			{
				device->setAudioDevice(audio);
				break;
			}
		}
		newDevices.append(device);
	}
	for (const auto &audio : std::as_const(audioList))
	{
		bool matched = false;
		for (const auto &device : newDevices)
		{
			if (device->hasAudio() && descriptionsMatch(device->name(), audio.description()))
			{
				matched = true;
				break;
			}
		}
		if (!matched)
		{
			auto device = new MediaDevice(this);
			device->setAudioDevice(audio);
			newDevices.append(device);
		}
	}
	
	// Replace the old device list.
	clearDevices();
	m_devices = newDevices;
}
