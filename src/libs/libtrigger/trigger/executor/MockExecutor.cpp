#include "MockExecutor.hpp"


#include "trigger/action/actions/NoOpAction.hpp"
#include "trigger/condition/conditions/AlwaysCondition.hpp"
#include "trigger/condition/conditions/NeverCondition.hpp"
#include "trigger/executor/TriggerExecutor.hpp"
#include "trigger/trigger/Trigger.hpp"

#include <QSharedPointer>


QSharedPointer<TriggerExecutor> mockTriggers(){
	auto ts=QSharedPointer<TriggerExecutor>::create();
	auto t1=QSharedPointer<Trigger>::create();
	auto t2=QSharedPointer<Trigger>::create();
	auto t3=QSharedPointer<Trigger>::create();
	t1->addCondition(QSharedPointer<AlwaysCondition>::create());
	t1->addCondition(QSharedPointer<NeverCondition>::create());
	t1->addAction(QSharedPointer<NoOpAction>::create());
	t1->addAction(QSharedPointer<NoOpAction>::create());
	
	t2->addCondition(QSharedPointer<AlwaysCondition>::create());
	t2->addAction(QSharedPointer<NoOpAction>::create());
	
	t3->addCondition(QSharedPointer<NeverCondition>::create());
	t3->addAction(QSharedPointer<NoOpAction>::create());
	ts->addTrigger(t1);
	ts->addTrigger(t2);
	ts->addTrigger(t3);
	
	for(int i=0; i<10; ++i) {
		auto t=QSharedPointer<Trigger>::create();
		t->addCondition(QSharedPointer<AlwaysCondition>::create());
		t->addAction(QSharedPointer<NoOpAction>::create());
		ts->addTrigger(t);
	}
	return ts;
}

