#include "SerialDeviceListModel.hpp"

#include "hw/actuators/SerialList.hpp"
#include "../libutil/utility/Standard.hpp"

SerialDeviceListModel::SerialDeviceListModel(QObject *parent)
	: QAbstractListModel(parent)
	, mList(new SerialList(this))
{
	//qDebug()<<" ctor";
	if(!connect(mList, &SerialList::serialDevicesChanged, this, &SerialDeviceListModel::onSerialDevicesChanged, OC_CONTYPE)){
		qWarning()<<"ERROR: could not connect";
	}
}

QVariant SerialDeviceListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	//qDebug()<<" header data asked for";
	return "";
}

int SerialDeviceListModel::rowCount(const QModelIndex &parent) const
{
	//qDebug()<<" row count asked for: "<<parent;
	return mList->count();
}

int SerialDeviceListModel::columnCount(const QModelIndex &) const
{
	//qDebug()<<" columncount asked for";
	return 1;
}

QVariant SerialDeviceListModel::data(const QModelIndex &index, int role) const
{
	//qDebug()<<" XXXXXXXXXXXXXXXXX Data asked for "<<index.row()<<" and role "<<role;
	if(Qt::DisplayRole ==role){
		QSerialPortInfo info=mList->device(index.row());
		QString str=info.portName()+" - "+info.systemLocation()+" - "+info.description()+" - "+info.manufacturer()+" - "+info.serialNumber();
		return QVariant(str);
	}
	return QVariant();
}


void SerialDeviceListModel::onSerialDevicesChanged()
{
	auto from= index(0), to=index(mList->count());
	//qDebug()<<"DEVICES CHANGED"	<< from << " -> " << to<< "( " << mList->count() << ")";
	emit dataChanged(from, to);
}
