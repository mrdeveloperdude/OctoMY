#include "HandshakeState.hpp"

HandshakeState::HandshakeState(bool i)
	: mInitiator(i)
	, mStep(VIRGIN)
{

}

bool HandshakeState::isVirgin() const
{
	return VIRGIN==mStep;
}
bool HandshakeState::isInitiator() const
{
	return mInitiator;
}
bool HandshakeState::isDone() const
{
	return ((quint8)mStep) >= ((quint8) ACK_OK);
}

HandshakeStep HandshakeState::nextStep() const
{
	switch(mStep) {
	case(VIRGIN):
		return mInitiator?SYN_OK:SYN_ACK_OK;
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
		qWarning()<<"ERROR: Trying to get next step after unknown step "<<QString::number(mStep);
		return IDLE_HANDSHAKE;
	}
}

HandshakeStep HandshakeState::step() const
{
	return mStep;
}
/*
void HandshakeState::bump()
{
	mStep=nextStep();
}*/

void HandshakeState::setSynOK()
{
	mStep=SYN_OK;
}

void HandshakeState::setSynAckOK()
{
	mStep=SYN_ACK_OK;
}

void HandshakeState::setAckOK()
{
	mStep=ACK_OK;
}

void HandshakeState::setInitiator(bool i)
{
	mInitiator=i;
}


QString handshakeStepToString(HandshakeStep step)
{
#define HANDSHAKE_STEP_TO_STRING_CASE(a) case(a): return (#a)
	switch(step) {
		HANDSHAKE_STEP_TO_STRING_CASE(VIRGIN);
		HANDSHAKE_STEP_TO_STRING_CASE(SYN_OK);
		HANDSHAKE_STEP_TO_STRING_CASE(SYN_ACK_OK);
		HANDSHAKE_STEP_TO_STRING_CASE(ACK_OK);
		HANDSHAKE_STEP_TO_STRING_CASE(IDLE_HANDSHAKE);
	default:
		return "UNKNOWN";
	}
#undef HANDSHAKE_STEP_TO_STRING_CASE
}
