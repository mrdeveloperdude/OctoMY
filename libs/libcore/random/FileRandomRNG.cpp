#include "FileRandomRNG.hpp"

#include <QDebug>

FileRandomRNG::FileRandomRNG(QString name, QString filename)
	: RNG(name)
	, file(filename)
	, ok(false)
{
	if(!file.exists()){
		qWarning()<<"ERROR: Could not open file for random source: "<<filename<<", reason: "<<file.errorString();
	}
	else if(file.isReadable()){
		qWarning()<<"ERROR: File for random source: "<<filename<<" is not readable";
	}
	else if(!file.open(QIODevice::ReadOnly)){
		qWarning()<<"ERROR: Could not open file source: "<<filename<<", reason: "<<file.errorString();
	}
	else {
		ok=true;
	}
}



//Return true if this source is dependant on any pseudo rng
inline bool FileRandomRNG::hasPseudo() {
	return false;
}

//Generates a random number on [0,0xffffffff]-interval
//This is the main generator function that all the others are based on
quint32 FileRandomRNG::generateInt32(void) {
	if(ok){
		union converter{
				char bytes[4];
				quint32 value;
		};
		converter conv;
		conv.value=0;
		qint64 ret=file.read(conv.bytes,4);
		if(4!=ret){
			qWarning()<<"ERROR: Read " << ret << " instead of 4 bytes from random source, random data may be tainted!";
		}
		return conv.value;
	}
	else{
		qWarning()<<"ERROR: There was an error";
		return 0;
	}
}
