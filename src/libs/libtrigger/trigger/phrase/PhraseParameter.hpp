#ifndef PHRASEPARAMETER_HPP
#define PHRASEPARAMETER_HPP


#include <QString>
#include <QVariant>

// An enumeration of parameter types. Extend as needed.
enum class PhraseParameterType {
	Player,
	Integer,
	UnitType
	// Add more types as required.
};

// A descriptor for a parameter that appears in the description template.
struct PhraseParameterDescriptor {
	QString name;              // The keyword, e.g. "player"
	PhraseParameterType type;  // The expected parameter type.
};


#endif // PHRASEPARAMETER_HPP
