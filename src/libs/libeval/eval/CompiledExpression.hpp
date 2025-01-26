#ifndef COMPILEDEXPRESSION_HPP
#define COMPILEDEXPRESSION_HPP


#include <QString>
#include <QSharedPointer>


struct ASTNode;
class Response;
class ExpressionContext;


class CompiledExpression {
	
private:
	QString original;
	QSharedPointer<ASTNode> rootNode;

public:
	QSharedPointer<Response> compile(const QString &original, const QSharedPointer<ASTNode> &astRoot);
	QSharedPointer<Response> execute(ExpressionContext &context, qreal &value) const;
	
	friend  QDebug operator<<(QDebug debug, const QSharedPointer<CompiledExpression> &expression);
};


QDebug operator<<(QDebug debug, const QSharedPointer<CompiledExpression> &expression);

#endif // COMPILEDEXPRESSION_HPP
