#ifndef EXPRESSIONSREGISTRY_H
#define EXPRESSIONSREGISTRY_H

#include "eval/CompiledExpression.hpp"
#include "eval/ExpressionContext.hpp"
#include "eval/ExpressionEvaluator.hpp"

#include <QHash>

class ExpressionsRegistry
{
private:
	struct ExpressionEntry {
		QSharedPointer<CompiledExpression> expression;
		qreal value{0.0};
	};

	ExpressionEvaluator evaluator;
	ExpressionContext context;
	QHash<QString, ExpressionEntry> expressions;

public:
	ExpressionsRegistry();

public:
	bool registerExpression(const QString &name, const QString &expression, bool replaceIfExists=true);
	bool removeExpression(const QString &name);
	QSharedPointer<CompiledExpression> lookupExpression(const QString &name) const;
	qreal lookupValue(const QString &name) const;
	void step(qreal dt);

};

#endif // EXPRESSIONSREGISTRY_H
