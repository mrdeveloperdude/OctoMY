#ifndef ACTIONCONTEXT_HPP
#define ACTIONCONTEXT_HPP

#include "uptime/SharedPointerWrapper.hpp"

class Trigger;
class TriggerExecutor;

class ActionContext
{
private:
	QSharedPointer<Trigger> mTrigger;
	QSharedPointer<TriggerExecutor> mExecutor;
public:
	ActionContext();

public:
	QSharedPointer<Trigger> trigger() const;
	void setTrigger(const QSharedPointer<Trigger> &trigger);
	
	QSharedPointer<TriggerExecutor> executor() const;
	void setExecutor(const QSharedPointer<TriggerExecutor> &executor);

};

#endif // ACTIONCONTEXT_HPP
