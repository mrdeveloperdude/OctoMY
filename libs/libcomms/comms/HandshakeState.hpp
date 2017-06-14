#ifndef HANDSHAKESTATE_HPP
#define HANDSHAKESTATE_HPP

#include <QDebug>

enum HandshakeRole {
	MASTER
	, SLAVE
};


enum HandshakeStep {
	VIRGIN // We are uninitialized
	, SYN_OK // Syn was sent(for masters)/ recieved(for slaves) OK
	, SYN_ACK_OK // Syn-ack was recieved(for masters)/ sent(for slaves) OK
	, ACK_OK // Ack was sent(for masters)/ recieved(for slaves) OK
	, IDLE_HANDSHAKE // Connection was established so no more steps
};

struct HandshakeState {
	HandshakeRole role;
	HandshakeStep step;


	HandshakeState(bool isMaster=true)
		: role(isMaster?MASTER:SLAVE)
		, step(VIRGIN)
	{

	}

	bool isVirgin() const
	{
		return VIRGIN==step;
	}
	bool isDone() const
	{
		return ACK_OK==step;
	}

	HandshakeStep nextStep()const
	{
		switch(step) {
		case(VIRGIN):
			return SYN_OK;
		case(SYN_OK):
			return SYN_ACK_OK;
		case(SYN_ACK_OK):
			return ACK_OK;
		case(ACK_OK):
			return IDLE_HANDSHAKE;
		case(IDLE_HANDSHAKE):
			qWarning()<<"ERROR: Trying to get next step after 'IDLE_HANDSHAKE'";
			return IDLE_HANDSHAKE;
		default:
			qWarning()<<"ERROR: Trying to get next step after unknown step "<<QString::number(step);
			return IDLE_HANDSHAKE;
		}
	}
};

#endif // HANDSHAKESTATE_HPP
