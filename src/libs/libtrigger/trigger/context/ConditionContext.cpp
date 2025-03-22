#include "ConditionContext.hpp"

#include "trigger/trigger/Trigger.hpp"
#include "trigger/executor/TriggerExecutor.hpp"


ConditionContext::ConditionContext() {}

QSharedPointer<Trigger> ConditionContext::trigger() const
{
	return mTrigger;
}

void ConditionContext::setTrigger(const QSharedPointer<Trigger> &trigger)
{
	mTrigger = trigger;
}

QSharedPointer<TriggerExecutor> ConditionContext::executor() const
{
	return mExecutor;
}

void ConditionContext::setExecutor(const QSharedPointer<TriggerExecutor> &executor)
{
	mExecutor = executor;
}
