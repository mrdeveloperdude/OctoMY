#include "LogDevice.hpp"

void LogDeviceNoop::setLogDirection(bool down){	
	Q_UNUSED(down);
}

void LogDeviceNoop::appendLog(const QString& text){
	Q_UNUSED(text);
}



