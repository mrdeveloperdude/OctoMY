#ifndef FUNCTIONREGISTRY_HPP
#define FUNCTIONREGISTRY_HPP

#include <QString>
#include <functional>
#include <QHash>
#include <QVector>

class FunctionRegistry {
private:
	using Function = std::function<qreal(const QVector<qreal> &)>;
	struct FunctionEntry {
		Function function;
		quint16 arity;
	};
	
	QHash<QString, FunctionEntry> functions;
	
public:
	FunctionRegistry();
	bool registerFunction(const QString &name, const Function &function, quint16 arity);
	bool removeFunction(const QString &name);
	Function lookupFunction(const QString &name) const;
	quint16 functionArity(const QString &name) const;
	
	void initializeDefaultFunctions();
};

#endif // FUNCTIONREGISTRY_HPP
