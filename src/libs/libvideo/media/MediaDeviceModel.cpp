#include "MediaDeviceModel.hpp"
#include "MediaDeviceManager.hpp"
#include "MediaDevice.hpp"
#include <QDebug>
#include <QIcon>

MediaDeviceModel::MediaDeviceModel(const QSharedPointer<MediaDeviceManager>& manager, QObject* parent)
	: QAbstractTableModel(parent)
	, m_manager(manager)
{
	// When devices change, update the model.
	connect(m_manager.data(), &MediaDeviceManager::devicesChanged, this, &MediaDeviceModel::onDevicesChanged);
	onDevicesChanged();
}

MediaDeviceModel::~MediaDeviceModel()
{
}

int MediaDeviceModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return filteredDevices().size();
}

int MediaDeviceModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	// We define six columns.
	return 5;
}

QVariant MediaDeviceModel::data(const QModelIndex &index, int role) const
{
	auto devices = filteredDevices();
	if (!index.isValid() || index.row() < 0 || index.row() >= devices.size()){
		return QVariant();
	}
	MediaDevice* device = devices.at(index.row());
	
	// For the first column, provide an icon in DecorationRole.
	if (role == Qt::DecorationRole && index.column() == 0) {
		if (device->previewActive()) {
			QPixmap pix = device->previewPixmap();
			if (!pix.isNull())
				return QIcon(pix);
			return QVariant();
		} else {
			if (device->hasAudio() && device->hasVideo())
				return QIcon(":/icons/media/cameramicrophone.svg");
			else if (device->hasAudio())
				return QIcon(":/icons/media/microphone.svg");
			else if (device->hasVideo())
				return QIcon(":/icons/media/camera.svg");
			else
				return QIcon(":/icons/controller/unknown.svg");
		}
	}
	// For DisplayRole, return column-specific data.
	if (role == Qt::DisplayRole) {
		switch (index.column()) {
		case 0:
			// Optionally, also display the name in column 0 if desired.
			//return device->name();
			return device->name();
		case 1: {
			if (device->hasAudio()) {
				// Use the channel count from the device's audio format.
				QAudioFormat format = device->audioFormat();
				int ch = format.channelCount();
				if (ch == 1)
					return QString("Mono");
				else if (ch == 2)
					return QString("Stereo");
				else
					return QString("%1 channels").arg(ch);
			}
			return QVariant();
		}
		case 2: {
			if (device->hasAudio()) {
				QAudioFormat format = device->audioFormat();
				return QString("%1 Hz").arg(format.sampleRate());
			}
			return QVariant();
		}
		case 3: {
			if (device->hasVideo()) {
				// Assuming MediaDevice::videoResolution() returns a QSize.
				QSize res = device->videoResolution();
				return QString("%1 x %2").arg(res.width()).arg(res.height());
			}
			return QVariant();
		}
		case 4: {
			if (device->hasVideo()) {
				// Assuming MediaDevice::videoFrameRate() returns a double.
				double fps = device->videoFrameRate();
				return QString("%1 fps").arg(fps, 0, 'f', 2);
			}
			return QVariant();
		}
		default:
			return QVariant();
		}
	}
	
	// For custom roles, fallback to existing behavior.
	switch (role) {
	case DevicePointerRole:
		return QVariant::fromValue(device);
	default:
		return QVariant();
	}
}

QVariant MediaDeviceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
		switch (section) {
		case 0: return QString("Device");
		case 1: return QString("Audio Channels");
		case 2: return QString("Sample Rate");
		case 3: return QString("Video Resolution");
		case 4: return QString("Frame Rate");
		default: return QVariant();
		}
	}
	return QAbstractTableModel::headerData(section, orientation, role);
}

void MediaDeviceModel::setFilter(DeviceTypeFilter filter)
{
	if (m_filter != filter) {
		beginResetModel();
		m_filter = filter;
		endResetModel();
	}
}

DeviceTypeFilter MediaDeviceModel::filter() const
{
	return m_filter;
}

void MediaDeviceModel::setPreviewActive(bool active)
{
	// Update preview status for each device in the filtered list.
	auto devices = filteredDevices();
	for (auto &device : std::as_const(devices)) {
		device->setPreviewActive(active);
	}
	// Trigger a refresh for all devices.
	emit dataChanged(index(0, 0), index(devices.size() - 1, columnCount()-1), { PreviewActiveRole, Qt::DecorationRole, Qt::DisplayRole });
}

void MediaDeviceModel::onDevicesChanged()
{
	beginResetModel();
	endResetModel();
	
	// (Re)connect to each device's preview updates.
	auto devices = m_manager->devices();
	for (auto &device : std::as_const(devices)) {
		connect(device, &MediaDevice::previewPixmapChanged,
				this, &MediaDeviceModel::onDevicePreviewChanged,
				Qt::UniqueConnection);
	}
}

void MediaDeviceModel::onDevicePreviewChanged()
{
	MediaDevice* device = qobject_cast<MediaDevice*>(sender());
	if (!device) {
		qWarning() << "Could not decipher sender";
		return;
	}
	auto devices = filteredDevices();
	int row = devices.indexOf(device);
	if (row >= 0) {
		emit dataChanged(index(row, 0), index(row, columnCount()-1), { Qt::DecorationRole, Qt::DisplayRole });
	}
}

QList<MediaDevice*> MediaDeviceModel::filteredDevices() const
{
	auto allDevices = m_manager->devices();
	QList<MediaDevice*> filtered;
	for (const auto &device : std::as_const(allDevices)) {
		bool include = false;
		switch (m_filter) {
		case DeviceTypeFilter::Any:
			include = true;
			break;
		case DeviceTypeFilter::OnlyAudio:
			include = device->hasAudio() && !device->hasVideo();
			break;
		case DeviceTypeFilter::OnlyVideo:
			include = device->hasVideo() && !device->hasAudio();
			break;
		case DeviceTypeFilter::AudioAndVideo:
			include = device->hasAudio() && device->hasVideo();
			break;
		}
		if (include){
			filtered.append(device);
		}
	}
	return filtered;
}
