#ifndef CHANCECONDITION_HPP
#define CHANCECONDITION_HPP

#include "trigger/condition/Condition.hpp"


class ChanceCondition : public ConditionBase
{
public:
	ChanceCondition();
	static QString staticName() { return QStringLiteral("Never"); }
	QString name() const override{ return staticName(); };
	bool actualEvaluate(ConditionContext &conditionContext) override;

};



#endif // CHANCECONDITION_HPP
