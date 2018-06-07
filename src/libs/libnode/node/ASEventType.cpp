#include "ASEventType.hpp"

#include "utility/Standard.hpp"

QDebug operator<< (QDebug d, ASEventType tt)
{
	OC_FUNCTIONGATE();
	switch(tt) {
#define TransactionType_case_stanza(A) case(A):{ d.nospace() << #A; }break
		TransactionType_case_stanza(AS_EVENT_CLEAR);
		TransactionType_case_stanza(AS_EVENT_GET);
		TransactionType_case_stanza(AS_EVENT_SET);
		TransactionType_case_stanza(AS_EVENT_LOAD);
		TransactionType_case_stanza(AS_EVENT_SAVE);
		TransactionType_case_stanza(AS_EVENT_GENERATE);
		TransactionType_case_stanza(AS_EVENT_SYNCHRONIZE);
		TransactionType_case_stanza(AS_EVENT_EXISTS);
		TransactionType_case_stanza(AS_EVENT_READY);
		TransactionType_case_stanza(AS_EVENT_DONE);
	default: {
		d.nospace() << "AS_UNKNOWN";
	}
	break;
#undef TransactionType_case_stanza
	}

	return d.space();
}


