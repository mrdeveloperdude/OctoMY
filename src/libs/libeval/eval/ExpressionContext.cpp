#include "ExpressionContext.hpp"

bool ExpressionContext::hasVariable(const QString &name) const{
	auto it = mVariables.find(name);
	return (it != mVariables.end());
}

qreal ExpressionContext::variable(const QString &name) const {
	auto it = mVariables.find(name);
	if (it != mVariables.end()) {
		return it.value();
	}
	return 0.0; // Default value if the variable is not found
}

void ExpressionContext::setVariable(const QString &name, qreal value) {
	mVariables[name] = value;
}

FunctionRegistry &ExpressionContext::functionRegistry() {
	return mFunctionRegistry;
}

