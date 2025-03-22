#ifndef PARAMETERTYPEREGISTRY_HPP
#define PARAMETERTYPEREGISTRY_HPP

#include <QString>
#include <QVariant>
#include <QPoint>
#include <functional>
#include <QHash>
#include <QList>

// Encapsulates the behavior of a parameter type.
struct ParameterType {
	QString typeName;
	// Converts a QVariant to its string representation.
	std::function<QString(const QVariant &)> toString;
	// Invokes an input method that shows a widget at mousePosition.
	// The input method should eventually call the provided callback with the new value.
	std::function<void(const QPoint &, const QVariant &, std::function<void(QVariant)>)> inputMethod;
	// Default value for this type.
	QVariant defaultValue;
};

// A registry that holds a mapping from type names to ParameterType objects.
class ParameterTypeRegistry {
private:
	QHash<QString, QSharedPointer<ParameterType> > m_types;
public:
	ParameterTypeRegistry() = default;
	
	void registerType(QSharedPointer<ParameterType> type);
	
	bool hasType(const QString &typeName) const;
	
	QSharedPointer<ParameterType> getType(const QString &typeName) const;
	
	QList<QSharedPointer<ParameterType> > listTypes() const;
	
};

#endif // PARAMETERTYPEREGISTRY_HPP
