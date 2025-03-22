#include "ParameterTypeRegistry.hpp"


void ParameterTypeRegistry::registerType(QSharedPointer<ParameterType> type) {
	if(type){
		m_types[type->typeName] = type;
	}
}

bool ParameterTypeRegistry::hasType(const QString &typeName) const {
	return m_types.contains(typeName);
}

// Returns the registered ParameterType. If not found, falls back to the "string" type.
QSharedPointer<ParameterType> ParameterTypeRegistry::getType(const QString &typeName) const {
	if (m_types.contains(typeName)){
		return m_types.value(typeName);
	}
	return m_types.value(QStringLiteral("string"));
}

QList<QSharedPointer<ParameterType> > ParameterTypeRegistry::listTypes() const {
	return m_types.values();
}
