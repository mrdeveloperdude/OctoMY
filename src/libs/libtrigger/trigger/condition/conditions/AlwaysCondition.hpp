#ifndef ALWAYSCONDITION_HPP
#define ALWAYSCONDITION_HPP

#include "trigger/condition/Condition.hpp"

class AlwaysCondition : public ConditionBase
{
public:
	AlwaysCondition();
	static QString staticName() { return QStringLiteral("Always"); }
	QString name() const override{ return staticName(); };
	bool actualEvaluate(ConditionContext &conditionContext) override;
};

#endif // ALWAYSCONDITION_HPP
