#include "Condition.hpp"

Condition::Condition(QString name, QString formula)
	: mName(name)
	, mFormula(formula)
{

}

void Condition::setValue(QString key, QString value)
{
	if(value.isEmpty() && mValues.contains(key)) {
		mValues.remove(key);
	} else {
		mValues[key]=value;
	}
}

// This is supposed to be overriden by the actual implementation for each condition
bool Condition::poll(ConditionContext &)
{
	return false;
}


QString Condition::toString()
{
	return QStringLiteral("CONDITION: ")	+mName +  QStringLiteral(" = ") + mFormula;
}
