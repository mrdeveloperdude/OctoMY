#ifndef ISYNCPARAMETER_HPP
#define ISYNCPARAMETER_HPP


#include <QDataStream>
#include <QString>


/*!
 * \brief The ISyncParameter interface defines the available operations for
 * sync parameters.
 *
 * Sync parameters are parameters that may be synchronized between noed in the
 * network, hence the name.
 *
 * The operations pertaining to the sync are as follows:
 *
 * Send my value to remote
 * Get my value from remote
 *
 * Send ack to remote
 * Receive ack from remote (expected after sending of value)
 *
 *
 */

class ISyncParameter
{
	// ISyncParameter interface
public:
	virtual quint16 bytes() const =0;					// The number of bytes taht this parameter will occupy in the send/receive stream
	virtual QString toString() const =0;				// Convert to a human readible representation in a string
	virtual QDataStream &operator<<(QDataStream &)=0;	// Receive remote's value
	virtual QDataStream &operator>>(QDataStream &)=0;	// Send my value to remote
	virtual bool hasPendingSync() const =0;				// Return true when we have data to send
	virtual bool hasPendingAck() const =0;				// Return true if we have ack to give
	virtual void ack() =0;								// Receive ack from remote

};


#endif // ISYNCPARAMETER_HPP
