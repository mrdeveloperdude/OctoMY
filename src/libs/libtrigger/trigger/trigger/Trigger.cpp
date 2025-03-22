#include "Trigger.hpp"

#include "trigger/action/Action.hpp"
#include "trigger/condition/Condition.hpp"
#include "trigger/condition/conditions/CompositeCondition.hpp"
#include "trigger/context/ActionContext.hpp"
#include "trigger/context/ConditionContext.hpp"
#include "utility/random/Random.hpp"

Trigger::Trigger()
	: mID{utility::random::randomString(10)}
	, mConditions{QSharedPointer<CompositeCondition>::create()}
	, mMinPollRate(1.0)
	, mMaxPollRate(60.0)

{
}

void Trigger::setEnabled(bool enabled)
{
	mEnabled = enabled;
}

bool Trigger::isEnabled() const
{
	return mEnabled;
}

void Trigger::setActive(bool active)
{
	mActive = active;
}

bool Trigger::isActive() const
{
	return mActive;
}

QString Trigger::id() const {
	return mID;
};

QVariant Trigger::toVariant() const
{
	return QVariant(mID);
}

void Trigger::addCondition(const QSharedPointer<ICondition> condition){
	mConditions->addCondition(condition);
}


const QSharedPointer<CompositeCondition> Trigger::conditions() const
{
	return mConditions;
}

void Trigger::addAction(const QSharedPointer<IAction> action)
{
	mActions.append(action);
}

const QList<QSharedPointer<IAction> > &Trigger::actions() const
{
	return mActions;
}

void Trigger::setMinPollRate(double rate)
{
	mMinPollRate = rate;
}

void Trigger::setMaxPollRate(double rate)
{
	mMaxPollRate = rate;
}

double Trigger::minPollRate() const
{
	return mMinPollRate;
}

double Trigger::maxPollRate() const
{
	return mMaxPollRate;
}

QSharedPointer<ICondition> Trigger::conditionByID(const QString &id) const{
	if(mConditions.isNull()){
		return nullptr;
	}
	if(mConditions->id() == id){
		return mConditions;
	}
	return mConditions->locate(id);
}


void Trigger::removeCondition(const QString &id){
	if(mConditions.isNull()){
		// No conditions to remove
		return;
	}
	if(mConditions->id() == id){
		// We cant remove top-level condition
		return;
	}
	return mConditions->remove(id);
}


QSharedPointer<IAction> Trigger::actionByID(const QString &id) const{
	for(auto action:mActions){
		if(action->id() == id){
			return action;
		}
	}
	return nullptr;
}

bool Trigger::checkConditions(QSharedPointer<ConditionContext> context){
	if(context.isNull()){
		qWarning() << "No context";
		return false;
	}
	if(!mConditions)
	{
		qWarning() << "No conditions";
		return false;
	}
	context->setTrigger(sharedFromThis());
	return mConditions->evaluate(*context);
}


bool Trigger::runActions(QSharedPointer<ActionContext> context){
	bool ok{true};
	if(context.isNull()){
		qWarning() << "No context";
		return false;
	}
	context->setTrigger(sharedFromThis());
	for(auto &action : mActions)
	{
		if(action){
			ok = ok && action->execute(*context);
		}
	}
	return ok;
}
