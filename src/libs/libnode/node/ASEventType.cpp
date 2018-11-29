#include "ASEventType.hpp"

#include "utility/Standard.hpp"



QString ASEventTypeToString(ASEventType tt)
{
	OC_FUNCTIONGATE();
	switch(static_cast<quint8>(tt)) {
#define ASEventType_case_stanza(A) case(A):{ return #A; }break
		ASEventType_case_stanza(AS_EVENT_NONE);
		ASEventType_case_stanza(AS_EVENT_STATUS);
		ASEventType_case_stanza(AS_EVENT_CLEAR);
		ASEventType_case_stanza(AS_EVENT_GET);
		ASEventType_case_stanza(AS_EVENT_SET);
		ASEventType_case_stanza(AS_EVENT_LOAD);
		ASEventType_case_stanza(AS_EVENT_SAVE);
		ASEventType_case_stanza(AS_EVENT_GENERATE);
		ASEventType_case_stanza(AS_EVENT_SYNCHRONIZE);
		ASEventType_case_stanza(AS_EVENT_EXISTS);
		ASEventType_case_stanza(AS_EVENT_COMPLETE);
	default: {
		return  QString("AS_EVENT_UNKNOWN(%1)").arg(tt);
	}
#undef TransactionType_case_stanza
	}
}



QDebug operator<< (QDebug d, ASEventType tt)
{
	OC_FUNCTIONGATE();
	QDebugStateSaver stateSaver(d);
	return d.noquote().nospace() << ASEventTypeToString(tt);
}
