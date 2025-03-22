#include "PreserveTriggerAction.hpp"


#include "trigger/context/ActionContext.hpp"
#include "trigger/trigger/Trigger.hpp"


PreserveTriggerAction::PreserveTriggerAction()
	: ActionBase("Preserve trigger.")
{
	
}

bool PreserveTriggerAction::execute(ActionContext &context)
{
	auto trigger = context.trigger();
	if(trigger){
		trigger->setActive(true);
		return true;
	}
	else{
		qWarning() << "No trigger";
	}
	return false;
}
