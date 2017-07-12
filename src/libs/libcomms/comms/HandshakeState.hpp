#ifndef HANDSHAKESTATE_HPP
#define HANDSHAKESTATE_HPP

#include <QDebug>


enum HandshakeStep:quint8 {
	VIRGIN // We are uninitialized
	, SYN_OK // Syn was sent(for initiators)/ recieved(for receivers) OK
	, SYN_ACK_OK // Syn-ack was recieved(for initiators)/ sent(for receivers) OK
	, ACK_OK // Ack was sent(for initiators)/ recieved(for receivers) OK
	, IDLE_HANDSHAKE // Connection was established so no more steps
};

class HandshakeState
{
private:
	bool mInitiator;
	HandshakeStep mStep;
public:

	HandshakeState(bool i=true);
	bool isVirgin() const;
	bool isInitiator() const;
	bool isDone() const;
	HandshakeStep nextStep() const;
	HandshakeStep step() const;
	//void bump();
	void setSynOK();
	void setSynAckOK();
	void setAckOK();
	void setInitiator(bool i);

};


QString handshakeStepToString(HandshakeStep step);

#endif // HANDSHAKESTATE_HPP
