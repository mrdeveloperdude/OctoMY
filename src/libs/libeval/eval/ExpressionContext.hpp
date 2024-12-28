#ifndef EXPRESSIONCONTEXT_HPP
#define EXPRESSIONCONTEXT_HPP

#include "FunctionRegistry.hpp"

#include <QHash>
#include <QString>

class ExpressionContext {
public:
	bool hasVariable(const QString &name) const;
	qreal variable(const QString &name) const;
	void setVariable(const QString &name, qreal value);
	
	FunctionRegistry &functionRegistry();
	
private:
	QHash<QString, qreal> mVariables;
	FunctionRegistry mFunctionRegistry;
};

#endif // EXPRESSIONCONTEXT_HPP
