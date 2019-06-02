#ifndef TRIGGERSET_HPP
#define TRIGGERSET_HPP

#include <QList>

class Trigger;
class ConditionContext;
class ActionContext;

class TriggerSet: public QList<Trigger *>
{

public:
	explicit TriggerSet();
	virtual ~TriggerSet();

public:
	bool executeTriggers(ConditionContext &cctx, ActionContext &actx);

};


#endif
// TRIGGERSET_HPP
