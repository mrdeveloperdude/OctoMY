#include "SerialDeviceListModel.hpp"

#include "hardware/serial/SerialList.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "uptime/ConnectionType.hpp"

#include <QDebug>

SerialDeviceListModel::SerialDeviceListModel(QObject *parent)
	: QAbstractListModel(parent)
	, mList(OC_NEW SerialList(this))
{
	OC_METHODGATE();
	//qDebug()<<" ctor";
	if(!connect(mList, &SerialList::serialDevicesChanged, this, &SerialDeviceListModel::onSerialDevicesChanged, OC_CONTYPE)) {
		qWarning()<<"ERROR: could not connect";
	}
}

QVariant SerialDeviceListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	OC_METHODGATE();
	//qDebug()<<" header data asked for";
	return "";
}

int SerialDeviceListModel::rowCount(const QModelIndex &parent) const
{
	OC_METHODGATE();
	//qDebug()<<" row count asked for: "<<parent;
	return mList->count();
}

int SerialDeviceListModel::columnCount(const QModelIndex &) const
{
	OC_METHODGATE();
	//qDebug()<<" columncount asked for";
	return 1;
}

QVariant SerialDeviceListModel::data(const QModelIndex &index, int role) const
{
	OC_METHODGATE();
	//qDebug()<<" XXXXXXXXXXXXXXXXX Data asked for "<<index.row()<<" and role "<<role;
	if(Qt::DisplayRole ==role) {
		QSerialPortInfo info=mList->device(index.row());
		QString str=info.portName()+" - "+info.systemLocation()+" - "+info.description()+" - "+info.manufacturer()+" - "+info.serialNumber();
		return QVariant(str);
	}
	return QVariant();
}


void SerialDeviceListModel::onSerialDevicesChanged()
{
	OC_METHODGATE();
	auto from= index(0), to=index(mList->count());
	//qDebug()<<"DEVICES CHANGED"	<< from << " -> " << to<< "( " << mList->count() << ")";
	emit dataChanged(from, to);
}
