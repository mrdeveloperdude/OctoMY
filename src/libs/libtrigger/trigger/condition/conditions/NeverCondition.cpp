#include "NeverCondition.hpp"


NeverCondition::NeverCondition()
	: ConditionBase("Never.")
{
}


bool NeverCondition::actualEvaluate(ConditionContext &conditionContext)
{
	Q_UNUSED(conditionContext);
	return false;
}
