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
