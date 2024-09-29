#ifndef SERIALDEVICELISTMODEL_HPP
#define SERIALDEVICELISTMODEL_HPP

#include <QAbstractListModel>
#include <QSerialPortInfo>


class SerialList;
class SerialDeviceListModel : public QAbstractListModel
{
	Q_OBJECT

private:
	SerialList *mList;

public:
	explicit SerialDeviceListModel(QObject *parent = 0);
	
public:
	// Header:
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	// Basic functionality:
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &) const;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	
	QSerialPortInfo deviceInfo(const QModelIndex &index) const;
	
public slots:
	void onSerialDevicesChanged();

signals:
	void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());
};

#endif
// SERIALDEVICELISTMODEL_HPP
