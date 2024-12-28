#include "FunctionRegistry.hpp"
#include <QtMath>


FunctionRegistry::FunctionRegistry(){
	initializeDefaultFunctions();
}

bool FunctionRegistry::registerFunction(const QString &name, const Function &function, quint16 arity) {
	if (functions.contains(name)) {
		return false; // Function with the same name already exists or invalid arity
	}
	functions.insert(name, {function, arity});
	return true;
}

bool FunctionRegistry::removeFunction(const QString &name) {
	return functions.remove(name) > 0;
}

FunctionRegistry::Function FunctionRegistry::lookupFunction(const QString &name) const {
	auto it = functions.find(name);
	if (it != functions.end()) {
		return it.value().function;
	}
	return nullptr;
}

quint16 FunctionRegistry::functionArity(const QString &name) const {
	auto it = functions.find(name);
	if (it != functions.end()) {
		return it.value().arity;
	}
	return 0;
}

void FunctionRegistry::initializeDefaultFunctions() {
	registerFunction("sin", [](const QVector<qreal> &args) { return qSin(args[0]); }, 1);
	registerFunction("cos", [](const QVector<qreal> &args) { return qCos(args[0]); }, 1);
	registerFunction("tan", [](const QVector<qreal> &args) { return qTan(args[0]); }, 1);
	registerFunction("pow", [](const QVector<qreal> &args) { return qPow(args[0], args[1]); }, 2);
	registerFunction("log", [](const QVector<qreal> &args) { return qLn(args[0]); }, 1);
}
