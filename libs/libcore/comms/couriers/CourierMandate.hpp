#ifndef COURIERMANDATE_HPP
#define COURIERMANDATE_HPP

#include <QDebug>
#include <QString>

struct CourierMandate {
	//Defaults reflect the common need; payload size must be specified, priority is casual, interval is 1 Hz and active is presumed to be yes by default
	CourierMandate(const quint16 payloadSize , const quint8 priority=10, const quint64 interval=1000, const bool receiveActive=true, const bool sendActive=true);
	//Number of bytes of desired payload from 0 to 400
	quint16 payloadSize;
	//Priority of this courier from 0 to 255 where lower number means lower
	// priority and couriers with the sam priority are treated equal
	quint8 priority;
	//Desired interval of sendings in milliseconds
	quint64 interval;
	//Are we currently active for receiving data?
	bool receiveActive;
	//Are we currently active for sending data?
	//NOTE: When courier reports "sendActive" that means that it wants to send,
	//      and so even if it writes 0 bytes, there will be a section id present
	//      in packet reserved for it with the 0 bytes following it
	bool sendActive;

	QString toString()const;

//	operator QString() const { return toString(); }
};



#endif // COURIERMANDATE_HPP
