#ifndef MEDIADEVICEMODEL_HPP
#define MEDIADEVICEMODEL_HPP

#include <QAbstractTableModel>
#include <QSharedPointer>
#include "MediaDeviceManager.hpp"

enum class DeviceTypeFilter {
	Any,
	OnlyAudio,
	OnlyVideo,
	AudioAndVideo
};

class MediaDevice;

class MediaDeviceModel : public QAbstractTableModel {
	Q_OBJECT
public:
	// Define custom roles.
	enum CustomRoles {
		DevicePointerRole = Qt::UserRole + 100
		, PreviewActiveRole
	};
	// We now use a table model with multiple columns.
	// Columns:
	// 0: Icon & Name
	// 1: Audio Channels
	// 2: Audio Sample Rate
	// 3: Video Resolution
	// 4: Video Frame Rate
	explicit MediaDeviceModel(const QSharedPointer<MediaDeviceManager>& manager, QObject* parent = nullptr);
	~MediaDeviceModel() override;
	
	// QAbstractTableModel overrides.
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	
	void setFilter(DeviceTypeFilter filter);
	DeviceTypeFilter filter() const;
	void setPreviewActive(bool active);
	
public slots:
	void onDevicesChanged();
	
private slots:
	void onDevicePreviewChanged();
	
private:
	QList<MediaDevice*> filteredDevices() const;
	QSharedPointer<MediaDeviceManager> m_manager;
	DeviceTypeFilter m_filter = DeviceTypeFilter::Any;
};

#endif // MEDIADEVICEMODEL_HPP
