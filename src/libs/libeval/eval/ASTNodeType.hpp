#ifndef ASTNODETYPE_HPP
#define ASTNODETYPE_HPP

#include <QString>

enum class ASTNodeType {
	Function,
	Variable,
	Operator,
	Number
};

QString ASTNodeTypeToString(const ASTNodeType type);

QDebug operator<<(QDebug debug, const ASTNodeType &type);

#endif // ASTNODETYPE_HPP
