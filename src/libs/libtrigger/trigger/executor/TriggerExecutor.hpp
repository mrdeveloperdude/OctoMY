#ifndef TRIGGEREXECUTOR_H
#define TRIGGEREXECUTOR_H

#include "uptime/SharedPointerWrapper.hpp"
#include "uptime/ConfigureHelper.hpp"

#include <QList>
#include <QSharedPointer>
#include <QTimer>


class Trigger;
class ConditionContext;
class ActionContext;

// TriggerExecutor drives the polling of triggers.
// It iterates over each trigger, evaluates its condition (which may reorder its composite children by cost),
// and then executes the actions if the condition is true.
class TriggerExecutor : public QObject, public QEnableSharedFromThis<TriggerExecutor>
{
	Q_OBJECT
private:
	QList<QSharedPointer<Trigger> > mTriggers;
	QSharedPointer<ConditionContext> mConditionContext;
	QSharedPointer<ActionContext> mActionContext;
	QTimer *mTimer;
	ConfigureHelper mConfigureHelper;

public:
	explicit TriggerExecutor(QObject *parent = nullptr);
	~TriggerExecutor() = default;

public:
	void configure();
	void addTrigger(const QSharedPointer<Trigger> &trigger);
	void removeTrigger(const QSharedPointer<Trigger> &trigger);
	QList<QSharedPointer<Trigger> > triggers() const;

public slots:
	void reset();
	void run(bool running);
	void step();

};

#endif // TRIGGEREXECUTOR_H
