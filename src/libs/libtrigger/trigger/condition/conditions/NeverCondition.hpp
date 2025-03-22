#ifndef NEVERCONDITION_H
#define NEVERCONDITION_H


#include "trigger/condition/Condition.hpp"

class NeverCondition : public ConditionBase
{
public:
	NeverCondition();
	static QString staticName() { return QStringLiteral("Never"); }
	QString name() const override{ return staticName(); };
	bool actualEvaluate(ConditionContext &conditionContext) override;

};


#endif // NEVERCONDITION_H
