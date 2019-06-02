#ifndef PACKETREADSTATE_HPP
#define PACKETREADSTATE_HPP


#include "CommsSessionIDType.hpp"


#include <QByteArray>
#include <QHostAddress>


class Key;
class QDataStream;

struct PacketReadState {

public:
	QByteArray datagram;
	QHostAddress remoteHost;
	quint16 remotePort;
	QSharedPointer<QDataStream> stream;

	SESSION_ID_TYPE multimagic;
	const int size;
	int totalAvailable;

	quint32 octomyProtocolMagic;
	//quint8 octomyProtocolFlags;
	quint32 octomyProtocolVersion;

	QByteArray octomyProtocolEncryptedMessage;
	QByteArray octomyProtocolDecryptedMessage;
	int octomyProtocolEncryptedMessageSize;
	int octomyProtocolDecryptedMessageSize;

	quint32 partMessageTypeID;
	quint16 partBytesAvailable;



	QSharedPointer<QDataStream> encStream;
	int encTotalAvailable;


	QByteArray octomyProtocolSenderIDRaw;
	int octomyProtocolSenderIDRawSize;
	QString octomyProtocolSenderID;

	SESSION_NONCE_TYPE octomyProtocolRemoteNonce;
	SESSION_NONCE_TYPE octomyProtocolReturnNonce;

	SESSION_ID_TYPE octomyProtocolDesiredRemoteSessionID;

	static const int OCTOMY_SENDER_ID_SIZE;
	static const int OCTOMY_ENCRYPTED_MESSAGE_SIZE;

public:
	PacketReadState(QByteArray datagram, QHostAddress remoteHost , quint16 remotePort);


public:
	QString toString();

public:

	// Read multimagic
	void readMultimagic();

	// Read protocol MAGIC
	void readProtocolMagic();

	/*
	// Read protocol FLAGS
	void readProtocolFlags();
	*/

	// Read protocol VERSION
	void readProtocolVersion();



	void readPartMessageTypeID();

	void readPartBytesAvailable();

	/////////////////////////////////////////// ENCRYPTED BEYOND THIS LINE


	void decrypt(Key &k);

	// Read Pub-key encrypted message body
	void readProtocolEncryptedMessage();

	// Extract full ID of sender
	void readEncSenderID();

	// Extract remote nonce
	void readEncRemoteNonce();


	// Extract return nonce
	void readEncReturnNonce();

	// Extract desired remote session ID
	void readEncDesiredRemoteSessionID();




};




#endif // PACKETREADSTATE_HPP
