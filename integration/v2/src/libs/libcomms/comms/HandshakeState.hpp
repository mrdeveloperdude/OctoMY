#ifndef HANDSHAKESTATE_HPP
#define HANDSHAKESTATE_HPP

#include <QDebug>

enum HandshakeStep:quint8 {
	SYN //START HANDSHAKE
	, SYN_ACK //CONTINUE HANDSHAKE
	, ACK //COMPLETE HANDSHAKE
	, NONE // HANDSHAKE IS DONE
};

class HandshakeState
{
private:
	bool mInitiator; // As opposed to Adherent
	quint8 mStepCounter;
	bool mAtLeastOneSend;
public:
	HandshakeState(bool i=true);
	bool isInitiator() const;
	bool isDone() const;
	bool isSending() const;
	bool isReceiving() const;
	// Report what step comms channel is currently supposed to transmit
	HandshakeStep expectedStepTX() const;
	// Report what step comms channel is currently expecting to receive
	HandshakeStep expectedStepRX() const;
	// Go to next transmit step and return true only if transmit step matches what is expected
	bool handleTX(HandshakeStep step);
	// Go to next receive step and return true only if received step matches what is expected
	bool handleRX(HandshakeStep step);
	void setInitiator(bool i);
	QString toString() const;
};


QString handshakeSendStepToString(HandshakeStep step);

#endif // HANDSHAKESTATE_HPP
