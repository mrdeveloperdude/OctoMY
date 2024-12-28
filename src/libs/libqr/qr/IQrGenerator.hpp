#ifndef IQRGENERATOR_HPP
#define IQRGENERATOR_HPP

#include "uptime/New.hpp"
#include "uptime/SharedPointerWrapper.hpp"

typedef quint8 QR_WIDTH_TYPE;

class IQrCode{
public:
	virtual QR_WIDTH_TYPE width() const = 0;
	virtual bool module(QR_WIDTH_TYPE x, QR_WIDTH_TYPE y) const = 0;
};


class IQrGenerator{
public:
	virtual void append(const QString &text) = 0;
	virtual QSharedPointer<IQrCode> generate() = 0;
};


#endif // IQRGENERATOR_HPP
