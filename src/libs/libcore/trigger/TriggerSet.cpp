#include "TriggerSet.hpp"

#include "Trigger.hpp"

#include "utility/Standard.hpp"

TriggerSet::TriggerSet()
//: QList<Courier *>()
{
	OC_METHODGATE();

}


TriggerSet::~TriggerSet()
{
	OC_METHODGATE();
	for(Trigger *trigger: *this) {
		(void)trigger;
		//trigger->deleteLater();
	}
	clear();
}




bool TriggerSet::executeTriggers(ConditionContext &cctx, ActionContext &actx)
{
	bool anyTrigger=false;
	for(Trigger *trigger: *this) {
		if(nullptr!=trigger) {
			const bool oneTrigger=trigger->executeTrigger(cctx,actx);
			anyTrigger=anyTrigger || oneTrigger;
		}
	}
	return anyTrigger;
}
