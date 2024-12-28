#ifndef TOKENTYPE_HPP
#define TOKENTYPE_HPP

#include <QDebug>


enum class TokenType { Whitespace, Number, Operator, Delimiter, Name, Parenthesis, End, Invalid };


QString TokenTypeToString(const TokenType type);

QDebug operator<<(QDebug debug, const TokenType &type);

#endif // TOKENTYPE_HPP
