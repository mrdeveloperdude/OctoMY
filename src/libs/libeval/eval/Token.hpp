#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "TokenType.hpp"

#include <QString>
#include <QRegularExpression>

struct Token {
	TokenType type;
	QString value;
	QString whitespace{""};
	int arity{0};
	QString toString() const{
		QString adapted{value};
		adapted.replace('\n', "\\n");
		adapted.replace('\t', "\\t");
		return QString("<") +  TokenTypeToString(type) + QString(":") + adapted + ">";
	}
	operator QString() const{
		return toString();
	}
};


inline bool operator==(const Token &lhs, const Token &rhs) {
	const auto type_equal = lhs.type == rhs.type;
	const auto value_equal = lhs.value == rhs.value;
	
	return type_equal && value_equal;
}

inline bool operator!=(const Token &lhs, const Token &rhs) {
	return !(lhs == rhs);
}


QDebug operator<<(QDebug debug, const Token &token);

#endif // TOKEN_HPP
