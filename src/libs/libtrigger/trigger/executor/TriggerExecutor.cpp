#include "TriggerExecutor.hpp"

#include "trigger/action/Action.hpp"
#include "trigger/condition/conditions/CompositeCondition.hpp"
#include "trigger/context/ActionContext.hpp"
#include "trigger/context/ConditionContext.hpp"
#include "trigger/trigger/Trigger.hpp"

#include <QDebug>
#include <QElapsedTimer>
#include <limits>

TriggerExecutor::TriggerExecutor(QObject *parent)
	: QObject(parent)
	, mConfigureHelper("TriggerExecutor",true, false, false)
{
	mTimer = new QTimer(this);
	connect(mTimer, &QTimer::timeout, this, &TriggerExecutor::step);
}


void TriggerExecutor::configure(){
	if(mConfigureHelper.configure()){
		mConditionContext = QSharedPointer<ConditionContext>::create();
		mConditionContext->setExecutor(sharedFromThis());
		mActionContext = QSharedPointer<ActionContext>::create();
		mActionContext->setExecutor(sharedFromThis());
	}
}


void TriggerExecutor::addTrigger(const QSharedPointer<Trigger> &trigger)
{
	if(mConfigureHelper.isConfiguredAsExpected()){
		mTriggers.append(trigger);
	}
}

void TriggerExecutor::removeTrigger(const QSharedPointer<Trigger> &trigger)
{
	if(mConfigureHelper.isConfiguredAsExpected()){
		mTriggers.removeAll(trigger);
	}
}


QList<QSharedPointer<Trigger> > TriggerExecutor::triggers() const{
	return mTriggers;
}



void TriggerExecutor::reset()
{
	if(mConfigureHelper.isConfiguredAsExpected()){
		for(auto &trig : mTriggers)
		{
			trig->setActive(true);
		}
	}
}

void TriggerExecutor::run(bool running)
{
	if(mConfigureHelper.isConfiguredAsExpected()){
		if(running){
			if(mTriggers.isEmpty()){
				return;
			}
			double globalMaxPollRate = std::numeric_limits<double>::max();
			for(const auto &trig : mTriggers)
			{
				if(!trig->isEnabled()){
					continue;
				}
				if(!trig->isActive()){
					continue;
				}
				if(trig->maxPollRate() < globalMaxPollRate){
					globalMaxPollRate = trig->maxPollRate();
				}
			}
			int interval = static_cast<int>(60000.0 / globalMaxPollRate);
			mTimer->start(interval);
		}
		else{
			mTimer->stop();
		}
	}
}

void TriggerExecutor::step()
{
	if(mConfigureHelper.isConfiguredAsExpected()){
		for(auto &trig : mTriggers)
		{
			if(!trig->isEnabled()){
				continue;
			}
			if(!trig->isActive()){
				continue;
			}
			// By default triggers only run once before being deactivated.
			// The preserve trigger action is used to re-activate triggers for perpetual running.
			trig->setActive(false);
			const auto conditionResult = trig->checkConditions(mConditionContext);
			if(conditionResult){
				trig->runActions(mActionContext);
			}
		}
	}
}
