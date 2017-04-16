#ifndef CONDITION_HPP
#define CONDITION_HPP

#include <QString>
#include <QHash>

class ConditionContext
{

};

class Condition
{
private:
	QString mName;
	QString mFormula;
	QHash<QString, QString> mValues;
public:
	Condition(QString name="", QString formula="");

public:
	void setValue(QString key, QString value);

	// Condition interface
public:
	virtual bool poll(ConditionContext &);

	virtual QString toString();

};

#endif // CONDITION_HPP

