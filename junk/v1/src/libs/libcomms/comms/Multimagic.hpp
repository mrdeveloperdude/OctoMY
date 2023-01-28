#ifndef MULTIMAGIC_HPP
#define MULTIMAGIC_HPP

#include "CommsSessionIDType.hpp"
#include "HandshakeState.hpp"

#include <QString>

enum Multimagic:SESSION_ID_TYPE {
	MULTIMAGIC_IDLE=0
	, MULTIMAGIC_SYN
	, MULTIMAGIC_SYNACK
	, MULTIMAGIC_ACK
	, MULTIMAGIC_LAST
};



QString MultimagicToString(SESSION_ID_TYPE m);

QString MultimagicToString(Multimagic m);


HandshakeStep multimagicToHandshakeStep(Multimagic);

#endif // MULTIMAGIC_HPP
