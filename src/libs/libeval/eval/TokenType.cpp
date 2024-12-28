#include "TokenType.hpp"

QString TokenTypeToString(const TokenType type) {
	switch (type) {
#define TokenTypeToStringStanza(A) case(TokenType::A): return QStringLiteral(#A); break
		TokenTypeToStringStanza(Whitespace);
		TokenTypeToStringStanza(Number);
		TokenTypeToStringStanza(Operator);
		TokenTypeToStringStanza(Delimiter);
		TokenTypeToStringStanza(Name);
		TokenTypeToStringStanza(Parenthesis);
		TokenTypeToStringStanza(End);
		TokenTypeToStringStanza(Invalid);
#undef TokenTypeToStringStanza
	}
	return "UNKNOWN";
}



QDebug operator<<(QDebug debug, const TokenType &type) {
	debug.nospace() << TokenTypeToString(type);
	return debug.space();
}
