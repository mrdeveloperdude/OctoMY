#ifndef PACKETSENDSTATE_HPP
#define PACKETSENDSTATE_HPP


#include "Multimagic.hpp"
#include "uptime/SharedPointerWrapper.hpp"

#include <QString>

class ReliabilitySystem;
class CommsSession;


struct PacketSendState {
public:
	QString mRemoteID;
	QSharedPointer<CommsSession> session;
	QByteArray datagram;
	QSharedPointer<QDataStream> stream;
	int bytesUsed;
	QSharedPointer<QDataStream> encStream;
	int encBytesUsed;
	QByteArray octomyProtocolUnencryptedMessage;
	QByteArray octomyProtocolEncryptedMessage;

	static const int OCTOMY_SENDER_ID_SIZE;
	static const int OCTOMY_ENCRYPTED_MESSAGE_SIZE;

public:
	PacketSendState();

	void setRemoteID(QString toID);

	void setSession( QSharedPointer<CommsSession> session);
	void writeMultimagic(Multimagic multimagic);

	void writeMultimagic(SESSION_ID_TYPE multimagic);

	void writeMagic();

	// Write a header with protocol "magic number" and a version
	void writeProtocolVersion();

	void writeSessionID(SESSION_ID_TYPE sessionID);
	void writeCourierID(quint32 courierID);

	void writePayloadSize(quint16 payloadSize);


	void writeReliabilityData(ReliabilitySystem &rs);


	/////////////////////////////////////////// ENCRYPTED BEYOND THIS LINE



	void encrypt();


	// Write Pub-key encrypted message body
	void writeProtocolEncryptedMessage();

	// Write full ID of sender
	void writeEncSenderID(QString senderID);

	// Write nonce
	void writeEncNonce(SESSION_NONCE_TYPE nonce);
	// Write desired remote session ID
	void writeEncSessionID(SESSION_ID_TYPE sessionID);

};


#endif
// PACKETSENDSTATE_HPP
