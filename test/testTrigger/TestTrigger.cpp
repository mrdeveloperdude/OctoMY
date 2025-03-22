#include "TestTrigger.hpp"

#include "test/Utility.hpp"
#include "trigger/TriggerManagerWidget.hpp"
#include "trigger/action/actions/CopyFileAction.hpp"
#include "trigger/condition/conditions/AlwaysCondition.hpp"
#include "trigger/condition/conditions/CompositeCondition.hpp"
#include "trigger/condition/conditions/GitRepoChangedCondition.hpp"
#include "trigger/condition/conditions/NeverCondition.hpp"
#include "trigger/executor/TriggerExecutor.hpp"
#include "trigger/factory/ActionFactory.hpp"
#include "trigger/factory/ConditionFactory.hpp"
#include "trigger/trigger/Trigger.hpp"
#include "uptime/New.hpp"
#include "uptime/SharedPointerWrapper.hpp"

void TestTrigger::testManager()
{
	Q_INIT_RESOURCE(icons);
	ActionFactory::instance().registerAll();
	ConditionFactory::instance().registerAll();
	auto tw{OC_NEW TriggerManagerWidget()};
	//const auto ts = mockTriggers();
	const auto ts = QSharedPointer<TriggerExecutor>::create();
	ts->configure();
	tw->configure(ts);
	tw->show();
	test::utility::waitForUIEnd(tw);
}


void TestTrigger::testCompositeCondition(){
	auto comp1 = QSharedPointer<CompositeCondition>::create();
	auto comp2 = QSharedPointer<CompositeCondition>::create();
	comp2->addCondition(QSharedPointer<AlwaysCondition>::create() );
	comp1->addCondition(comp2);
	comp1->addCondition(QSharedPointer<NeverCondition>::create() );
	comp1->toHTML("test");
}


void TestTrigger::testVermaster(){
	Q_INIT_RESOURCE(icons);
	ActionFactory::instance().registerAll();
	ConditionFactory::instance().registerAll();
	auto tw{OC_NEW TriggerManagerWidget()};
	const auto ts = QSharedPointer<TriggerExecutor>::create();
	ts->configure();
	auto t1=QSharedPointer<Trigger>::create();
	t1->addCondition(QSharedPointer<GitRepoChangedCondition>::create());
	t1->addAction(QSharedPointer<CopyFileAction>::create());
	ts->addTrigger(t1);
	
	tw->configure(ts);
	tw->show();
	test::utility::waitForUIEnd(tw);
}


OC_TEST_MAIN(test, TestTrigger)

