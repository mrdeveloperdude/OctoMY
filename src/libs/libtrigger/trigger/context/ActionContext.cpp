#include "ActionContext.hpp"

#include "trigger/trigger/Trigger.hpp"
#include "trigger/executor/TriggerExecutor.hpp"


ActionContext::ActionContext() {}


QSharedPointer<Trigger> ActionContext::trigger() const
{
	return mTrigger;
}

void ActionContext::setTrigger(const QSharedPointer<Trigger> &trigger)
{
	mTrigger = trigger;
}

QSharedPointer<TriggerExecutor> ActionContext::executor() const
{
	return mExecutor;
}

void ActionContext::setExecutor(const QSharedPointer<TriggerExecutor> &executor)
{
	mExecutor = executor;
}
