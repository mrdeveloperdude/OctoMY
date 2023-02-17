#include "QHexEditDataDevice.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

QHexEditDataDevice::QHexEditDataDevice(QHexEditData *hexeditdata): QIODevice(hexeditdata), _hexeditdata(hexeditdata)
{
	OC_METHODGATE();
	this->_reader = OC_NEW QHexEditDataReader(hexeditdata, this);
	this->_writer = OC_NEW QHexEditDataWriter(hexeditdata, this);
}

qint64 QHexEditDataDevice::size() const
{
	OC_METHODGATE();
	return this->_hexeditdata->length();
}

qint64 QHexEditDataDevice::readData(char *data, qint64 maxlen)
{
	OC_METHODGATE();
	if(!this->_hexeditdata) {
		return 0;
	}

	QByteArray ba = this->_reader->read(this->pos(), maxlen);

	if(!ba.isEmpty()) {
		memcpy(data, ba.data(), static_cast<size_t>(ba.length()));
		return ba.length();
	}

	return -1;
}

qint64 QHexEditDataDevice::writeData(const char *data, qint64 len)
{
	OC_METHODGATE();
	if(!this->_hexeditdata) {
		return 0;
	}

	this->_writer->replace(this->pos(), QByteArray::fromRawData(data, static_cast<int>(len)));
	return len; /* Append if needed */
}
