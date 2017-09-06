#include "PacketReadState.hpp"

#include <QDataStream>

#include "security/Key.hpp"
#include "utility/Standard.hpp"

#include "Multimagic.hpp"

const int PacketReadState::OCTOMY_SENDER_ID_SIZE=32;
const int PacketReadState::OCTOMY_ENCRYPTED_MESSAGE_SIZE=36;


PacketReadState::PacketReadState(QByteArray datagram, QHostAddress remoteHost , quint16 remotePort)
	: datagram(datagram)
	, remoteHost(remoteHost)
	, remotePort(remotePort)
	, stream(OC_NEW QDataStream(&this->datagram, QIODevice::ReadOnly))
	, multimagic(0)
	, size(datagram.size())
	, totalAvailable(size)
	, octomyProtocolMagic(0)
	  //, octomyProtocolFlags(0)
	, octomyProtocolVersion(0)
	, octomyProtocolEncryptedMessageSize(0)
	, octomyProtocolDecryptedMessageSize(0)
	, partMessageTypeID(0)
	, partBytesAvailable(0)
	, encStream()
	, encTotalAvailable(0)
	, octomyProtocolSenderIDRawSize(0)
	, octomyProtocolRemoteNonce(INVALID_NONCE)
	, octomyProtocolReturnNonce(INVALID_NONCE)
	, octomyProtocolDesiredRemoteSessionID(INVALID_SESSION_ID)
{

}


QString PacketReadState::toString()
{
	QString out;
	{
		QTextStream ts(&out);
		ts << "remoteHost="<<remoteHost.toString()<<":"<<QString::number(remotePort);
		ts << ", multimagic="<<MultimagicToString(multimagic);
	}
	return out;
}

// Read multimagic
void PacketReadState::readMultimagic()
{
	*stream >> multimagic;
	totalAvailable-=sizeof(multimagic);
}

// Read protocol MAGIC
void PacketReadState::readProtocolMagic()
{
	*stream >> octomyProtocolMagic;
	totalAvailable-=sizeof(octomyProtocolMagic);
}

/*
// Read protocol FLAGS
void PacketReadState::readProtocolFlags()
{
	*stream >> octomyProtocolFlags;
	totalAvailable-=sizeof(octomyProtocolFlags);
}
*/

// Read protocol VERSION
void PacketReadState::readProtocolVersion()
{
	*stream >> octomyProtocolVersion;
	totalAvailable-=sizeof(octomyProtocolVersion);
}



void PacketReadState::readPartMessageTypeID()
{
	partMessageTypeID=0;
	*stream >> partMessageTypeID;
	totalAvailable-=sizeof(partMessageTypeID);
}

void PacketReadState::readPartBytesAvailable()
{
	partBytesAvailable=0;
	*stream >> partBytesAvailable;
	totalAvailable-=sizeof(partBytesAvailable);
}

/////////////////////////////////////////// ENCRYPTED BEYOND THIS LINE



void PacketReadState::decrypt(Key &k)
{
	if(k.isValid(true)) {
		if(octomyProtocolEncryptedMessageSize>0) {
#ifdef DO_CC_ENC
			octomyProtocolDecryptedMessage=k.decrypt(octomyProtocolEncryptedMessage);
#else
			octomyProtocolDecryptedMessage=octomyProtocolEncryptedMessage;
#endif
			octomyProtocolDecryptedMessageSize=octomyProtocolDecryptedMessage.size();
			if(octomyProtocolDecryptedMessageSize>0) {
				encStream=QSharedPointer<QDataStream> (OC_NEW QDataStream(&this->octomyProtocolDecryptedMessage, QIODevice::ReadOnly));
				if(encStream.isNull()) {
					qWarning()<<"ERROR: Could not allocate encStream";
				} else {
					encTotalAvailable=octomyProtocolEncryptedMessageSize;
				}
			} else {
				qWarning()<<"ERROR: Source text is empty while decrypting";
			}
		} else {
			qWarning()<<"ERROR: Cipher text is empty while decrypting";
		}
	} else {
		qWarning()<<"ERROR: Invalid key while decrypting";
	}


}

// Read Pub-key encrypted message body
void PacketReadState::readProtocolEncryptedMessage()
{
	if(!encStream.isNull()) {
		*stream >> octomyProtocolEncryptedMessage;
		qWarning()<<"RX CIPHERTEXT WAS: "<<octomyProtocolEncryptedMessage;
		octomyProtocolEncryptedMessageSize=octomyProtocolEncryptedMessage.size();
		totalAvailable-=octomyProtocolEncryptedMessageSize;
	} else {
		qWarning()<<"ERROR: encStream was nullptr";
	}
}


// Extract full ID of sender
void PacketReadState::readEncSenderID()
{
	if(!encStream.isNull()) {
		octomyProtocolSenderIDRaw.clear();
		*encStream >> octomyProtocolSenderIDRaw;
		octomyProtocolSenderIDRawSize=octomyProtocolSenderIDRaw.size();
		encTotalAvailable-=octomyProtocolSenderIDRawSize;
		octomyProtocolSenderID=octomyProtocolSenderIDRaw.toHex().toUpper();
	} else {
		qWarning()<<"ERROR: encStream was nullptr";
	}
}


// Extract remote nonce
void PacketReadState::readEncRemoteNonce()
{
	if(!encStream.isNull()) {
		octomyProtocolRemoteNonce=INVALID_NONCE;
		*encStream >> octomyProtocolRemoteNonce;
		encTotalAvailable-=sizeof(octomyProtocolRemoteNonce);
	} else {
		qWarning()<<"ERROR: encStream was nullptr";
	}

}


// Extract return nonce
void PacketReadState::readEncReturnNonce()
{
	if(!encStream.isNull()) {
		octomyProtocolReturnNonce=INVALID_NONCE;
		*encStream >> octomyProtocolReturnNonce;
		encTotalAvailable-=sizeof(octomyProtocolReturnNonce);
	} else {
		qWarning()<<"ERROR: encStream was nullptr";
	}
}


// Extract desired remote session ID
void PacketReadState::readEncDesiredRemoteSessionID()
{
	if(!encStream.isNull()) {
		octomyProtocolDesiredRemoteSessionID=INVALID_SESSION_ID;
		*encStream >> octomyProtocolDesiredRemoteSessionID;
		encTotalAvailable-=sizeof(octomyProtocolDesiredRemoteSessionID);
	} else {
		qWarning()<<"ERROR: encStream was nullptr";
	}
}
