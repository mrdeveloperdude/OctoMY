#include "ASTNodeType.hpp"

#include <QDebug>

QString ASTNodeTypeToString(const ASTNodeType type) {
	switch (type) {
#define ASTNodeTypeToStringStanza(A) case(ASTNodeType::A): return QStringLiteral(#A); break
		ASTNodeTypeToStringStanza(Function);
		ASTNodeTypeToStringStanza(Variable);
		ASTNodeTypeToStringStanza(Operator);
		ASTNodeTypeToStringStanza(Number);
#undef ASTNodeTypeToStringStanza
	}
	return "UNKNOWN";
}



QDebug operator<<(QDebug debug, const ASTNodeType &type) {
	debug.nospace() << ASTNodeTypeToString(type);
	return debug.space();
}
