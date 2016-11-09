#ifndef CLIENTSIGNATURE_HPP
#define CLIENTSIGNATURE_HPP


#include <QHostAddress>

#include "basic/NetworkAddress.hpp"

class Client;


/**

  ClientSignature is a minimal identification for clients that is used solely by
  CommsChannel.

  The signature contains an ID and the public network address used in
  communication. ClientSignature is mutable, and may be updated.

  It accomodates the special need for uniquely identifying
  the communicating clients using only 8 bytes to represent their identity, as
  opposed to the 512 bytes used in a full ID string.

  This is accomplished by converting the first 16 HEX digits of the full ID
  string into one 64 bit unsigned int.

 */

class ClientSignature
{

private:

	quint64 mShortHandID;
	QString mFullID;
	NetworkAddress mAddress;

public:
	explicit ClientSignature();
	ClientSignature(const ClientSignature &other);
	explicit ClientSignature(const quint64 &shortHandID, const NetworkAddress &address);
	explicit ClientSignature(const QString &fullID, const NetworkAddress &address);
	explicit ClientSignature(const QString &fullID);

	const QString toString() const;

	quint64 shortHandID() const;
	QString fullID() const;
	NetworkAddress address() const;

	void setAddress(const NetworkAddress &address);
	void setShortHandID(const quint64 &shortHandID);
	void setFullID(const QString &fullID);

	bool isValid() const;




};


#endif // CLIENTSIGNATURE_HPP
