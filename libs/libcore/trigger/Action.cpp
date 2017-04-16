#include "Action.hpp"

Action::Action(QString name)
	: mName(name)
{

}


// Action interface
void Action::perform(ActionContext &)
{

}


QString Action::toString()
{
	return QStringLiteral("ACTION: ")	+mName;
}
