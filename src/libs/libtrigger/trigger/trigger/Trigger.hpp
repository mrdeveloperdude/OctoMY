#ifndef TRIGGER_HPP
#define TRIGGER_HPP

#include "uptime/SharedPointerWrapper.hpp"

#include <QList>
#include <QSharedPointer>
#include <QVariantMap>

class CompositeCondition;
class IAction;
class ICondition;
class ConditionContext;
class ActionContext;


// A Trigger holds one condition tree (which may be composite) and a list of actions.
// It also defines minimum and maximum poll rates (in polls per minute).
class Trigger: public QEnableSharedFromThis<Trigger>
{
private:
//	QString mName;
	QString mID;
	bool mEnabled{true};
	bool mActive{true};
	QSharedPointer<CompositeCondition> mConditions;
	QList<QSharedPointer<IAction> > mActions;
	double mMinPollRate; // polls per minute
	double mMaxPollRate; // polls per minute
	
public:
	Trigger();
	
	void setEnabled(bool enabled);
	bool isEnabled() const;
	void setActive(bool active);
	bool isActive() const;
	QString id() const;
	QVariant toVariant() const;
	
	void addCondition(const QSharedPointer<ICondition> condition);
	const QSharedPointer<CompositeCondition> conditions() const;
	void addAction(const QSharedPointer<IAction> action);
	const QList<QSharedPointer<IAction> > &actions() const;
	
	// Poll rate in polls per minute.
	void setMinPollRate(double rate);
	void setMaxPollRate(double rate);
	double minPollRate() const;
	double maxPollRate() const;
	QSharedPointer<ICondition> conditionByID(const QString &id) const;
	QSharedPointer<IAction> actionByID(const QString &id) const;
	
	void removeCondition(const QString &id);
	
	bool checkConditions(QSharedPointer<ConditionContext> context);
	bool runActions(QSharedPointer<ActionContext> context);

};


#endif // TRIGGER_HPP
