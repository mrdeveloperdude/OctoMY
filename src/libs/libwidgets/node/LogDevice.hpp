#ifndef LOGDEVICE_HPP
#define LOGDEVICE_HPP

#include <QString>

class LogDevice{
public:
	virtual void setDirection(bool down) = 0;
	virtual void appendLog(const QString& text) =0;
};

class LogDeviceNoop:public LogDevice{
	void setDirection(bool down) override;
	void appendLog(const QString& text) override;
};

#endif // LOGDEVICE_HPP

