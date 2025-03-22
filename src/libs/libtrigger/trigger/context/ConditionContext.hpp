#ifndef CONDITIONCONTEXT_HPP
#define CONDITIONCONTEXT_HPP

#include "uptime/SharedPointerWrapper.hpp"

class Trigger;
class TriggerExecutor;

class ConditionContext
{
private:
	QSharedPointer<Trigger> mTrigger;
	QSharedPointer<TriggerExecutor> mExecutor;
public:
	ConditionContext();

public:
	QSharedPointer<Trigger> trigger() const;
	void setTrigger(const QSharedPointer<Trigger> &trigger);
	
	QSharedPointer<TriggerExecutor> executor() const;
	void setExecutor(const QSharedPointer<TriggerExecutor> &executor);
};

#endif // CONDITIONCONTEXT_HPP
