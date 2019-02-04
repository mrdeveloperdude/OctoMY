#include "PacketSendState.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"


const int PacketSendState::OCTOMY_SENDER_ID_SIZE=20;
const int PacketSendState::OCTOMY_ENCRYPTED_MESSAGE_SIZE=20;


PacketSendState::PacketSendState()
	: session(nullptr)
	, stream(OC_NEW QDataStream(&this->datagram, QIODevice::WriteOnly))
	, bytesUsed(0)
	, encStream(OC_NEW QDataStream(&this->octomyProtocolUnencryptedMessage, QIODevice::WriteOnly))
	, encBytesUsed(0)
{
	OC_METHODGATE();
}

void PacketSendState::setRemoteID(QString toID)
{
	OC_METHODGATE();
	mRemoteID=toID;
}

void PacketSendState::setSession( QSharedPointer<CommsSession> session)
{
	OC_METHODGATE();
	this->session=session;
}

void PacketSendState::writeMultimagic(Multimagic multimagic)
{
	OC_METHODGATE();
	const SESSION_ID_TYPE multimagicInt=static_cast<SESSION_ID_TYPE>(multimagic);
	writeMultimagic(multimagicInt);
}

void PacketSendState::writeMultimagic(SESSION_ID_TYPE multimagic)
{
	OC_METHODGATE();
	*stream << multimagic;
	bytesUsed += sizeof(multimagic);
}

void PacketSendState::writeMagic()
{
	OC_METHODGATE();
	const quint32 magic=static_cast<quint32>(OCTOMY_PROTOCOL_MAGIC);//Protocol MAGIC identifier
	*stream << magic;
	bytesUsed += sizeof(magic);
}

// Write a header with protocol "magic number" and a version
void PacketSendState::writeProtocolVersion()
{
	OC_METHODGATE();
	const quint32 version=static_cast<qint32>(OCTOMY_PROTOCOL_VERSION_CURRENT);//Protocol version
	*stream << version;
	bytesUsed += sizeof(version);
	stream->setVersion(version); //Qt Datastream version
}

void PacketSendState::writeSessionID(SESSION_ID_TYPE sessionID)
{
	OC_METHODGATE();
	*stream << sessionID;
	bytesUsed += sizeof(sessionID);
}

void PacketSendState::writeCourierID(quint32 courierID)
{
	OC_METHODGATE();
	*stream << courierID;
	bytesUsed += sizeof(courierID);
}

void PacketSendState::writePayloadSize(quint16 payloadSize)
{
	OC_METHODGATE();
	*stream << payloadSize;
	bytesUsed += sizeof(payloadSize);
}


void PacketSendState::writeReliabilityData(ReliabilitySystem &rs)
{
	OC_METHODGATE();
	Q_UNUSED(rs);
#ifdef USE_RELIBABILITY_SYSTEM
	// Write reliability data
	// TODO: incorporate writing logic better into reliability class
	// TODO: convert types used in reliability system to Qt for portability piece of mind
	ReliabilitySystem &rs=localClient->reliabilitySystem();
	*stream << rs.localSequence();
	bytesUsed += sizeof(unsigned int);
	*stream << rs.remoteSequence();
	bytesUsed += sizeof(unsigned int);
	*stream << rs.generateAckBits();
	bytesUsed += sizeof(unsigned int);
#endif
	//sizeof(quint32 /*magic*/) +sizeof(quint32 /*version*/) +sizeof(quint64 /*short ID*/) +sizeof(unsigned int /*sequence*/)+sizeof(unsigned int /*ack*/)+sizeof(unsigned int /*ack bits*/)+sizeof(quint32 /*message type*/)
	//qDebug()<<"SEND GLOBAL HEADER SIZE: "<<bytesUsed;
	// Send using courier.
	//NOTE: When courier reports "sendActive" that means that it wants to send,
	//      and so even if it writes 0 bytes, there will be a section id present
	//      in packet reserved for it with the 0 bytes following it

}


/////////////////////////////////////////// ENCRYPTED BEYOND THIS LINE



void PacketSendState::encrypt()
{
	OC_METHODGATE();
	if(nullptr==session) {
		qWarning()<<"ERROR: No session while encrypting";
		return;
	}
	auto k=session->key();
	if(nullptr==k) {
		qWarning()<<"ERROR: No key while encrypting";
		return;
	}
	if(!k->isValid(true)) {
		qWarning()<<"ERROR: Invalid key while encrypting";
		return;
	}
	if(octomyProtocolUnencryptedMessage.size()<1) {
		qWarning()<<"ERROR: Source text is empty while encrypting";
		return;
	}
#ifdef DO_CC_ENC
	octomyProtocolEncryptedMessage=k.encrypt(octomyProtocolUnencryptedMessage);
#else
	octomyProtocolEncryptedMessage=octomyProtocolUnencryptedMessage;
#endif
}


// Write Pub-key encrypted message body
void PacketSendState::writeProtocolEncryptedMessage()
{
	OC_METHODGATE();
	auto size=octomyProtocolEncryptedMessage.size();
	qWarning()<<"TX CIPHERTEXT WAS: "<<octomyProtocolEncryptedMessage;
	*stream << octomyProtocolEncryptedMessage;
	bytesUsed-=size;
}

// Write full ID of sender
void PacketSendState::writeEncSenderID(QString senderID)
{
	OC_METHODGATE();
	const auto raw=QByteArray::fromHex(senderID.toUtf8());
	const auto size=raw.size();
	*encStream << raw;
	encBytesUsed+=size;
}

// Write nonce
void PacketSendState::writeEncNonce(SESSION_NONCE_TYPE nonce)
{
	OC_METHODGATE();
	*encStream << nonce;
	encBytesUsed+=sizeof(nonce);
}

// Write desired remote session ID
void PacketSendState::writeEncSessionID(SESSION_ID_TYPE sessionID)
{
	OC_METHODGATE();
	*encStream << sessionID;
	encBytesUsed+=sizeof(sessionID);
}
