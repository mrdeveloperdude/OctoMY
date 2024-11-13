#include "NameMapping.hpp"

#include <QVariant>

NameMapping::NameMapping()
{
	
}

QVariantMap NameMapping::toVariantMap() const{
	QVariantMap out;
	for (auto it = begin(); it != end(); ++it) {
		out.insert(it.key(), QVariant(it.value()));  // Explicitly wrap QString in QVariant
	}
	return out;
}

void NameMapping::fromVariantMap(const QVariantMap &variantMap) {
	clear();
	for (auto it = variantMap.begin(); it != variantMap.end(); ++it) {
		if (it.value().canConvert<QString>()) {
			insert(it.key(), it.value().toString());
		}
	}
}
