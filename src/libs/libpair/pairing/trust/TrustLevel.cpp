#include "TrustLevel.hpp"


QString trustLevelToString(TrustLevel level)
{
#define  TrustLevelToStringCASE(A) case (A):return #A
	switch(level) {
		TrustLevelToStringCASE(TRUST);
		TrustLevelToStringCASE(BLOCK);
		TrustLevelToStringCASE(REMOVE);
		default:
		TrustLevelToStringCASE(IGNORE);
	}
#undef TrustLevelToStringCASE
	return "UNKNOWN";
}


TrustLevel trustLevelFromString(const QString &string)
{
#define  TrustLevelFromStringCASE(A) if(#A == string){return A;}
	TrustLevelFromStringCASE(TRUST);
	TrustLevelFromStringCASE(BLOCK);
	TrustLevelFromStringCASE(REMOVE);
	TrustLevelFromStringCASE(IGNORE);
#undef TrustLevelFromStringCASE
	return IGNORE;
}


QDebug operator<<(QDebug debug, TrustLevel level) {
	debug.nospace() << trustLevelToString(level);
	return debug.space();
}
