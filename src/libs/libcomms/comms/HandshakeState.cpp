#include "HandshakeState.hpp"

HandshakeState::HandshakeState(bool i)
	: mInitiator(i)
	, mStepCounter(0)
{

}



/*



TODO:
	keep int step ctr and return enum for tx step & rx step
								 TODO: Add timer to session to keep track of how long since last handshake TX/RX
									 Use this time data when selecting delay in cc.reschedule() & al.

										 NOTE: Detect step mismatch by role vs role check and resolve it with duel as is now. This avoid all the special cases in checking if a step is valid or not.


*/



bool HandshakeState::isInitiator() const
{
	return mInitiator;
}

bool HandshakeState::isDone() const
{
	return ( (!isSending()) && (!expectedStepRX()) );
}

bool HandshakeState::isSending() const
{
	return (NONE!=expectedStepTX());
}

bool HandshakeState::isReceiving() const
{
	return (NONE!=expectedStepRX());
}


HandshakeStep HandshakeState::expectedStepTX() const
{
	if(mInitiator) {
		switch(mStepCounter) {
		case(0):
			return SYN;
		case(2):
			return ACK;
		}
	} else {
		switch(mStepCounter) {
		case(1):
			return SYN_ACK;
		}
	}
	return NONE;
}



HandshakeStep HandshakeState::expectedStepRX() const
{
	if(mInitiator) {
		switch(mStepCounter) {
		case(1):
			return SYN_ACK;
		}
	} else {
		switch(mStepCounter) {
		case(0):
			return SYN;
		case(2):
			return ACK;
		}
	}
	return NONE;
}

bool HandshakeState::handleRX(HandshakeStep step)
{
	if(NONE == step){
		return false;
	}
	const bool ok = ( expectedStepRX() == step );
	if(ok){
		mStepCounter++;
	}
	return ok;
}

void HandshakeState::setInitiator(bool i)
{
	mInitiator=i;
}


QString HandshakeState::toString() const
{
	return "expectedStepTX="+handshakeSendStepToString(expectedStepTX()) +", expectedStepRX=" +handshakeSendStepToString(expectedStepRX()) +"("+QString::number(mStepCounter)+"):"+(mInitiator?"INITIATOR":"ADHERENT") ;
}



QString handshakeSendStepToString(HandshakeStep step)
{
#define HANDSHAKE_SEND_STEP_TO_STRING_CASE(a) case(a): return (#a)
	switch(step) {
		HANDSHAKE_SEND_STEP_TO_STRING_CASE(SYN);
		HANDSHAKE_SEND_STEP_TO_STRING_CASE(SYN_ACK);
		HANDSHAKE_SEND_STEP_TO_STRING_CASE(ACK);
		HANDSHAKE_SEND_STEP_TO_STRING_CASE(NONE);
	default:
		return "UNKNOWN";
	}
#undef HANDSHAKE_SEND_STEP_TO_STRING_CASE
}
