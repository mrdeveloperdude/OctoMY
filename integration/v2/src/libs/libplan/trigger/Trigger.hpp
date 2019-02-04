#ifndef TRIGGER_HPP
#define TRIGGER_HPP


#include <QString>
#include <QVariant>

class Condition;
class ConditionContext;
class Action;
class ActionContext;


class Trigger
{
private:
	QString mName;
	QList<Condition *> mConditions;
	QList<Action *> mActions;
	bool mEnabled;
public:
	Trigger(QString name="NONAME");
public:
	void addAction(Action &action);
	void addCondition(Condition &condition);
	void removeAction(Action &action);
	void removeCondition(Condition &condition);

	QList<Condition *> conditions();
	QList<Action *> actions();

	void setEnabled(bool enabled);
	bool isEnabled();


	QVariant toVariant();

	// This is the main method. Will execute actions if pollTriggers returns true
	bool executeTrigger(ConditionContext &cctx, ActionContext &actx);

	// Poll all conditions and return the result
	bool pollConditions(ConditionContext &cctx);

	// Execute all actions regardless of trigger status
	void executeActions(ActionContext &actx);
};


#endif // TRIGGER_HPP
