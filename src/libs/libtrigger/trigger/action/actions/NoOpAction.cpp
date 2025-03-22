#include "NoOpAction.hpp"

#include "trigger/context/ActionContext.hpp"


NoOpAction::NoOpAction()
	: ActionBase("Do nothing.")
{
	
}

bool NoOpAction::execute(ActionContext &)
{
	// Intentionally does nothing.
	return true;
}
