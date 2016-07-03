#ifndef CLIENTSIGNATURE_HPP
#define CLIENTSIGNATURE_HPP


#include <QHostAddress>

class Client;


/**
 *  \brief Communication between OctoMY clients can be challenging, especially
 *  with many clients interconnected in complex ways.
 *
 * Clients need a way to identify eachother as well as the level of trust that
 * exists between them.
 *
 * The ClientSignature class represents all the identification and trust that
 * this client has for another connected client.
 *
 * Identification usually starts with an initial network address (IPv4/hostname)
 * and is later enriched during different handshakes to hold network-neutral id,
 * security tokens and alternate network addresses.
 *
 */

class ClientSignature{

	public:

		quint32 platform;
		quint32 executable;
		QHostAddress host;
		quint16 port;

	public:
		explicit ClientSignature(const quint32 platform=0, const quint32 executable=0, const QHostAddress host=QHostAddress(), const quint16 port=0);


		const QString toString() const;


};


#endif // CLIENTSIGNATURE_HPP
