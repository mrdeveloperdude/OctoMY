#include "Multimagic.hpp"


QString MultimagicToString(Multimagic m)
{
#define MULTIMAGIC_TO_STRING_CASE(a) case(a): return(#a)
	switch(m) {
		MULTIMAGIC_TO_STRING_CASE(MULTIMAGIC_IDLE);
		MULTIMAGIC_TO_STRING_CASE(MULTIMAGIC_SYN);
		MULTIMAGIC_TO_STRING_CASE(MULTIMAGIC_SYNACK);
		MULTIMAGIC_TO_STRING_CASE(MULTIMAGIC_ACK);
		MULTIMAGIC_TO_STRING_CASE(MULTIMAGIC_LAST);
	default:
		return "MULTIMAGIC_"+QString::number((SESSION_ID_TYPE)m);

	}
#undef MULTIMAGIC_TO_STRING_CASE
}

QString MultimagicToString(SESSION_ID_TYPE m)
{
	return MultimagicToString((Multimagic)m);
}



HandshakeStep multimagicToHandshakeStep(Multimagic multimagi)
{
	switch(multimagi) {
	default:
	case(MULTIMAGIC_LAST):
	case(MULTIMAGIC_IDLE):
		return NONE;
	case(MULTIMAGIC_SYN):
		return SYN;
	case(MULTIMAGIC_SYNACK):
		return SYN_ACK;
	case(MULTIMAGIC_ACK):
		return ACK;
	}

}
