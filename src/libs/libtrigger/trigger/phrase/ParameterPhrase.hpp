#ifndef PARAMETERPHRASE_HPP
#define PARAMETERPHRASE_HPP


#include "uptime/SharedPointerWrapper.hpp"

#include <QString>
#include <QHash>
#include <QList>
#include <QVariant>
#include <QRegularExpression>

// Forward declare the registry.
class ParameterTypeRegistry;

// A descriptor for a parameter as parsed from the phrase.
struct PhraseParameterDescriptor {
	QString name;
	QString type; // e.g. "local-path", "string", etc.
};

class ParameterPhrase {
private:
	const QRegularExpression mVariableRegex;
	QString mTemplateText;
	QHash<QString, QVariant> mParameterValues;
	QList<PhraseParameterDescriptor> mDescriptors;
	// Pointer to the registry for parameter types.
	QSharedPointer<ParameterTypeRegistry> mRegistry;

public:
	static const QString separator;
public:
	explicit ParameterPhrase(const QString &templateText, QSharedPointer<ParameterTypeRegistry> registry = nullptr);

	void prepareDefaultValues();
	void setParameterValue(const QString &paramName, const QVariant &value);
	QVariant parameterValue(const QString &paramName) const;
	QString templateText() const;
	QString filledPhrase(const QString &prefix = QString()) const;
	QList<PhraseParameterDescriptor> parameterDescriptors() const;
	QList<PhraseParameterDescriptor> parseDescriptors() const;
};

#endif // PARAMETERPHRASE_HPP
