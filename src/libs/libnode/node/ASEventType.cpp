#include "ASEventType.hpp"

#include "utility/Standard.hpp"

QDebug operator<< (QDebug d, ASEventType tt)
{
	OC_FUNCTIONGATE();
	switch(tt) {
#define ASEventType_case_stanza(A) case(A):{ d.nospace() << #A; }break
		ASEventType_case_stanza(AS_EVENT_STATUS);
		ASEventType_case_stanza(AS_EVENT_CLEAR);
		ASEventType_case_stanza(AS_EVENT_GET);
		ASEventType_case_stanza(AS_EVENT_SET);
		ASEventType_case_stanza(AS_EVENT_LOAD);
		ASEventType_case_stanza(AS_EVENT_SAVE);
		ASEventType_case_stanza(AS_EVENT_GENERATE);
		ASEventType_case_stanza(AS_EVENT_SYNCHRONIZE);
		ASEventType_case_stanza(AS_EVENT_EXISTS);
		ASEventType_case_stanza(AS_EVENT_DONE);
	default: {
		d.nospace() << "AS_EVENT_UNKNOWN";
	}
	break;
#undef TransactionType_case_stanza
	}

	return d.space();
}


