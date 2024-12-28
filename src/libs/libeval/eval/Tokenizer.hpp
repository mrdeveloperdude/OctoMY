#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <QString>
#include <QVector>
#include <QSharedPointer>

#include "Token.hpp"
class Response;

class Tokenizer {
public:
	QSharedPointer<Response> tokenize(const QString &source, QVector<Token> &tokens);
};


#endif // TOKENIZER_HPP
