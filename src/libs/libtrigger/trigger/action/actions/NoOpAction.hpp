#ifndef NOOPACTION_HPP
#define NOOPACTION_HPP

#include "trigger/action/Action.hpp"

class ActionContext;


class NoOpAction : public ActionBase
{
public:
	NoOpAction();
	static QString staticName() { return QStringLiteral("NoOp"); }
	QString name() const override{ return staticName(); };
	bool execute(ActionContext &) override;
};

#endif // NOOPACTION_HPP
