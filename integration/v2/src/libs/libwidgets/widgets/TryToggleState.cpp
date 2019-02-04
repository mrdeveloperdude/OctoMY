#include "TryToggleState.hpp"

#include "uptime/MethodGate.hpp"

#include <QString>
#include <QDebug>

QString ToggleStateToSTring(TryToggleState s)
{
	OC_FUNCTIONGATE();
#define  ToggleStateToStringCASE(A) case (A):return #A
	switch(s) {
		ToggleStateToStringCASE(OFF);
		ToggleStateToStringCASE(GOING_ON);
		ToggleStateToStringCASE(ON);
		ToggleStateToStringCASE(GOING_OFF);

		//default:
	}
#undef ToggleStateToStringCASE
	return "UNKNOWN";
}




QDebug operator<<(QDebug d, const TryToggleState &s)
{
	OC_FUNCTIONGATE();
	d.nospace() << ToggleStateToSTring(s);
	return d.space();
}



