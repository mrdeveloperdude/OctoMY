#include "TrustLevel.hpp"

QString trustLevelToString(TrustLevel level)
{
#define  TrustLevelToStringCASE(A) case (A):return #A
	switch(level) {
		TrustLevelToStringCASE(TRUST);
		TrustLevelToStringCASE(BLOCK);
		// default:
		TrustLevelToStringCASE(IGNORE);
	}
#undef TrustLevelToStringCASE
	return "UNKNOWN";
}
