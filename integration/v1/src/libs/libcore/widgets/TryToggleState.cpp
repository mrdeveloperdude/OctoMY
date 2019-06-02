#include "TryToggleState.hpp"

#include "utility/Standard.hpp"

#include <QString>
#include <QDebug>

QString ToggleStateToSTring(TryToggleState s)
{
	OC_FUNCTIONGATE();
#define  ToggleStateToStringCASE(A) case (A):return #A
	switch(s) {
	default:
		return "UNKNOWN";
		ToggleStateToStringCASE(OFF);
		ToggleStateToStringCASE(GOING_ON);
		ToggleStateToStringCASE(ON);
		ToggleStateToStringCASE(GOING_OFF);
	}
#undef ToggleStateToStringCASE
}




QDebug operator<<(QDebug d, const TryToggleState &s)
{
	OC_FUNCTIONGATE();
	d.nospace() << ToggleStateToSTring(s);
	return d.space();
}



