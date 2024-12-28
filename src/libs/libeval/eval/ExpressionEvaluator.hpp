#ifndef EXPRESSIONEVALUATOR_HPP
#define EXPRESSIONEVALUATOR_HPP

#include "ExpressionContext.hpp"


#include <QMap>
#include <QVector>
#include <QSharedPointer>
#include <QString>

class Response;
class CompiledExpression;

class ExpressionEvaluator {
public:
	QSharedPointer<Response> eval(ExpressionContext &context, const QString &expression, qreal &result);
	QSharedPointer<Response> compile(const QString &expression, QSharedPointer<CompiledExpression> &compiledExpression);
};

#endif // EXPRESSIONEVALUATOR_HPP
