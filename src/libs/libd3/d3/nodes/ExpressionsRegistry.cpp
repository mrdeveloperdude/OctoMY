#include "ExpressionsRegistry.hpp"


#include "eval/Response.hpp"

#include <QDebug>

ExpressionsRegistry::ExpressionsRegistry() {}

bool ExpressionsRegistry::registerExpression(const QString &name, const QString &expression, bool replaceIfExists) {
	if (!replaceIfExists && expressions.contains(name)) {
		qWarning() << "Expression already existed for name " << name;
		return false;
	}
	QSharedPointer<CompiledExpression> compiledExpression;
	const auto response = evaluator.compile(expression, compiledExpression);
	if(!response->success){
		qWarning() << "Failed to compile expression: " << expression;
		qWarning() << "           Error message was: " << response->message;
		return false;
	}
	expressions.insert(name, {compiledExpression, 0.0});
	return true;
}

bool ExpressionsRegistry::removeExpression(const QString &name) {
	return expressions.remove(name) > 0;
}

QSharedPointer<CompiledExpression> ExpressionsRegistry::lookupExpression(const QString &name) const {
	auto it = expressions.find(name);
	if (it != expressions.end()) {
		return it.value().expression;
	}
	return nullptr;
}

qreal ExpressionsRegistry::lookupValue(const QString &name) const {
	auto it = expressions.find(name);
	if (it != expressions.end()) {
		return it.value().value;
	}
	return 0.0;
}



void ExpressionsRegistry::step(qreal dt){
	auto exp = expressions.values();
	context.setVariable("dt", dt);
	for(auto &e:exp){
		e.expression->execute(context, e.value);
	}
}
