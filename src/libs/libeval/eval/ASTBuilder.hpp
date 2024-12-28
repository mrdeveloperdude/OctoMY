#ifndef ASTBUILDER_HPP
#define ASTBUILDER_HPP


#include <QVector>
#include <QSharedPointer>
#include "Token.hpp"


struct ASTNode;
class Response;


QSharedPointer<Response> rpn(const QVector<Token> &tokens, QQueue<Token> &output);

QSharedPointer<Response> ast(const QQueue<Token> &rpnQueue, QSharedPointer<ASTNode> &astRoot);

class ASTBuilder {
public:
	QSharedPointer<Response> build(const QVector<Token> &tokens, QSharedPointer<ASTNode> &astRoot);
};

#endif // ASTBUILDER_HPP
