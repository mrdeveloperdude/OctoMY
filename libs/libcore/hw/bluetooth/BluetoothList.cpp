#include "BluetoothList.hpp"

#include <QtBluetooth/QBluetoothLocalDevice>

BluetoothList::BluetoothList(QObject *parent) : QObject(parent)
{

}

QString BluetoothList::toSpecStanzas(QString space){
	QString out="";
	QBluetoothLocalDevice localDevice;
	if (localDevice.isValid()) {
		out+=space+"bluetooth {\n";
		out+=space+"\tdescription=\""+localDevice.name()+"\"\n";
		out+=space+"\tid=\""+localDevice.address().toString()+"\"\n";
		out+=space+"}\n\n";
	}
	return out;

}

