#include "ChanceCondition.hpp"

#include "utility/random/Random.hpp"

ChanceCondition::ChanceCondition()
	: ConditionBase("At a chance of [percent]%.")
{
}


bool ChanceCondition::actualEvaluate(ConditionContext &conditionContext)
{
	Q_UNUSED(conditionContext);
	const auto variant = phrase()->parameterValue("percent");
	bool ok{false};
	quint8 percent = variant.toUInt(&ok);
	if(!ok){
		qWarning() << "Could not parse percent from" << variant.toString();
		return false;
	}
	return ((utility::random::qrand() % 100) <= percent);
}
