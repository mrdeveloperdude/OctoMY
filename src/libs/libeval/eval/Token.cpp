#include "Token.hpp"

#include <QDebug>

QDebug operator<<(QDebug debug, const Token &token) {
	debug.nospace() << token.toString();
	return debug.space();
}
