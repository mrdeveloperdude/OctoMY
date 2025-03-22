#include "AlwaysCondition.hpp"

AlwaysCondition::AlwaysCondition()
	: ConditionBase("Always.")
{
}


bool AlwaysCondition::actualEvaluate(ConditionContext &conditionContext)
{
	Q_UNUSED(conditionContext);
	return true;
}

