#ifndef QHEXEDITDATADEVICE_H
#define QHEXEDITDATADEVICE_H

#include <QtCore>
#include "QHexEditData.hpp"
#include "QHexEditDataReader.hpp"
#include "QHexEditDataWriter.hpp"

class QHexEditDataDevice : public QIODevice
{
	public:
		QHexEditDataDevice(QHexEditData* hexeditdata);
		virtual qint64 size() const;

	protected:
		qint64 readData(char *data, qint64 maxlen);
		qint64 writeData(const char *data, qint64 len);

	private:
		QHexEditData* _hexeditdata;
		QHexEditDataReader* _reader;
		QHexEditDataWriter* _writer;
};

#endif // QHEXEDITDATADEVICE_H
