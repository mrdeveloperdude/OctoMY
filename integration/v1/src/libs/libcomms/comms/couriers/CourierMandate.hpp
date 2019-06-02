#ifndef COURIERMANDATE_HPP
#define COURIERMANDATE_HPP

#include <QDebug>
#include <QString>

/*!
 * \brief The CourierMandate struct is how CommsChannel can choose which Couriers
 * to go with for each sending opportunity.
 *
 * Each courier will provide CommsChanel with its mandate uppon request, and
 * CommsChannel will then sort the Couriers by the values in the mandate to
 * select which Courier get their turn and when.
 *
 * NOTE: It is implied that the values of the mandate DOES NOT CHANGE between
 * calls to Courier::mandate() otherwise this convention will fall appart.
 * TODO: Look into ways to avoid this
 */

struct CourierMandate {
	// Defaults reflect the common need; payload size must be specified, priority is casual, nextSend  "yesterday" and active is presumed to be yes by default
	CourierMandate(const quint16 payloadSize , const quint8 priority=10, const quint64 nextSend=1337, const bool receiveActive=true, const bool sendActive=true);
	// Number of bytes of desired payload from 0 to 400
	quint16 payloadSize;
	// Priority of this courier from 0 to 255 where lower number means lower
	// priority and couriers with the same priority are treated equal
	quint8 priority;
	// Desired time of next sending in milliseconds
	quint64 nextSend;
	// Are we currently active for receiving data?
	bool receiveActive;
	// Are we currently active for sending data?
	// NOTE: When courier reports "sendActive" that means that it wants to send,
	//       and so even if it writes 0 bytes, there will be a section id present
	//       in packet reserved for it with the 0 bytes following it
	bool sendActive;

	QString toString()const;

//	operator QString() const { return toString(); }
};



#endif // COURIERMANDATE_HPP
