#include "Trigger.hpp"

#include "Condition.hpp"
#include "Action.hpp"

Trigger::Trigger(QString name)
	: mName(name)
	, mEnabled(true)
{

}


void Trigger::addAction(Action &action)
{
	if(!mActions.contains(&action)) {
		mActions.push_back(&action);
	}
}

void Trigger::addCondition(Condition &condition)
{
	if(!mConditions.contains(&condition)) {
		mConditions.push_back(&condition);
	}
}


void Trigger::removeAction(Action &action)
{
	if(mActions.contains(&action)) {
		mActions.removeAll(&action);
	}
}

void Trigger::removeCondition(Condition &condition)
{
	if(mConditions.contains(&condition)) {
		mConditions.removeAll(&condition);
	}
}


QList<Condition *> Trigger::conditions()
{
	return mConditions;

}

QList<Action *> Trigger::actions()
{
	return mActions;
}


void Trigger::setEnabled(bool enabled)
{
	mEnabled=enabled;
}

bool Trigger::isEnabled()
{
	return mEnabled;
}



QVariant Trigger::toVariant()
{
	return QVariant(mName);
}



// This is the main method. Will execute actions if pollTriggers returns true
bool Trigger::executeTrigger(ConditionContext &cctx, ActionContext &actx)
{
	const bool triggerStatus=pollConditions(cctx);
	if(triggerStatus) {
		executeActions(actx);
	}
	return triggerStatus;
}

// Poll all conditions and return the result
bool Trigger::pollConditions(ConditionContext &cctx)
{
	bool status=true;
	for(Condition *condition:mConditions) {
		if(nullptr!=condition){
			const bool ret=condition->poll(cctx);
			status = status && ret;
		}
	}
	return status;
}

// Execute all actions regardless of trigger status
void Trigger::executeActions(ActionContext &actx)
{
	for(Action *action:mActions) {
		if(nullptr!=action){
			action->perform(actx);
		}
	}
}
