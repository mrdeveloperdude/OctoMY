#ifndef PRESERVETRIGGERACTION_HPP
#define PRESERVETRIGGERACTION_HPP




#include "trigger/action/Action.hpp"

class ActionContext;


class PreserveTriggerAction : public ActionBase
{
public:
	PreserveTriggerAction();
	static QString staticName() { return QStringLiteral("PreserveTrigger"); }
	QString name() const override{ return staticName(); };
	bool execute(ActionContext &) override;
};

#endif // PRESERVETRIGGERACTION_HPP
