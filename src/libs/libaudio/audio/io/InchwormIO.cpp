#include "InchwormIO.hpp"

#include <QtGlobal>


qint64 InchwormIO::readData(char *data, qint64 maxlen){
	auto len=mBuffer.readData(data, maxlen);
	if(len>0){
		emit samplesNeeded(mBuffer);
	}
	return len;
}




qint64 InchwormIO::writeData(const char *data, qint64 maxlen){
	auto len=mBuffer.writeData(data, maxlen);
	if(len>0){
		emit samplesAvailable(mBuffer);
	}
	return len;
}

