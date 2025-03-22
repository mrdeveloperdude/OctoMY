#include "ParameterPhrase.hpp"

#include "ParameterTypeRegistry.hpp"

#include <QDebug>

const QString ParameterPhrase::separator{"|"};

ParameterPhrase::ParameterPhrase(const QString &templateText, QSharedPointer<ParameterTypeRegistry> registry)
	: mVariableRegex("\\[(?<name>[^:\\]]+)(?::(?<type>[^\\]]+))?\\]")
	, mTemplateText(templateText)
	, mRegistry(registry)
{
	mDescriptors = parseDescriptors();
	prepareDefaultValues();
}


void ParameterPhrase::prepareDefaultValues(){
	
	// Initialize parameter values using the registry’s default value if available.
	for (const auto &descriptor : std::as_const(mDescriptors)) {
		if (mRegistry && mRegistry->hasType(descriptor.type)) {
			auto typeInfo = mRegistry->getType(descriptor.type);
			setParameterValue(descriptor.name, typeInfo->defaultValue);
		} else {
			setParameterValue(descriptor.name, "");
		}
	}
}

void ParameterPhrase::setParameterValue(const QString &paramName, const QVariant &value)
{
	mParameterValues[paramName] = value;
}

QVariant ParameterPhrase::parameterValue(const QString &paramName) const
{
	return mParameterValues.value(paramName);
}

QString ParameterPhrase::templateText() const
{
	return mTemplateText;
}

QString ParameterPhrase::filledPhrase(const QString &linkPrefix) const
{
	QString filled = mTemplateText;
	// For each stored parameter, replace its token with its string representation.
	for (auto it = mParameterValues.constBegin(); it != mParameterValues.constEnd(); ++it) {
		QString key = it.key();
		QString valueStr = it.value().toString();
		QString typeName = "string";
		// Find the type from the parsed descriptors.
		for (const auto &desc : mDescriptors) {
			if (desc.name == key) {
				typeName = desc.type;
				break;
			}
		}
		// If a registry is available, use the type’s conversion function.
		if (mRegistry && mRegistry->hasType(typeName)) {
			auto typeInfo = mRegistry->getType(typeName);
			valueStr = typeInfo->toString(it.value());
		}
		// Show a placeholder if no value is set.
		if (valueStr.isEmpty())
			valueStr = key; // QStringLiteral("[not set]");
		// Replace tokens in the form [name:type] and fallback [name].
		if(linkPrefix.isEmpty()){
			filled.replace(QString("[%1:%2]").arg(key, typeName), QString("<b>%1</b>").arg(valueStr));
			filled.replace(QString("[%1]").arg(key), QString("<b>%1</b>").arg(valueStr));
		}
		else{
			filled.replace(QString("[%1:%2]").arg(key, typeName), QString("<a href=\"%1\">%2</a>").arg(linkPrefix + key, valueStr));
			filled.replace(QString("[%1]").arg(key), QString("<a href=\"%1\">%2</a>").arg(linkPrefix + key, valueStr));
		}
	}
	return filled;
}


QList<PhraseParameterDescriptor> ParameterPhrase::parameterDescriptors() const
{
	return mDescriptors;
}


QList<PhraseParameterDescriptor> ParameterPhrase::parseDescriptors() const
{
	QList<PhraseParameterDescriptor> list;
	auto it = mVariableRegex.globalMatch(mTemplateText);
	while (it.hasNext()) {
		const auto match = it.next();
		const auto name = match.captured("name").trimmed();
		const auto type = match.captured("type").trimmed();
		list.append({ name, type.isEmpty() ? "string" : type});
	}
	return list;
}

