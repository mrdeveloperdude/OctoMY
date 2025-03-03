#ifndef LOGDEVICE_H
#define LOGDEVICE_H

#include <QString>

class LogDevice{
public:
	virtual void setLogDirection(bool down) = 0;
	virtual void appendLog(const QString& text) =0;
};

class LogDeviceNoop:public LogDevice{
	void setLogDirection(bool down) override;
	void appendLog(const QString& text) override;
};

#endif // LOGDEVICE_H

