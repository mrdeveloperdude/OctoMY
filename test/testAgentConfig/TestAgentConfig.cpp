#include "TestAgentConfig.hpp"

#include <QSharedPointer>

#include "agent/AgentConfig.hpp"
#include "agent/AgentConfigStore.hpp"
#include "utility/string/QVariantPrettyPrinter.hpp"


void TestAgentConfig::testAgentConfig()
{
	QVariantPrettyPrinter printer;
	auto qdbg = qDebug().noquote().nospace();
	qdbg << "\n";
	AgentConfig ac;
	
	auto map1 = ac.toMap();
	qdbg << "\ntoMap from empty:\n" << printer.prettyPrint(map1);
	
	QVariantMap map2;
	map2["integer"] = 1337;
	map2["string"] = "LEET";
	map2["bool"] = true;
	qdbg << "\nirrelevant map:\n" << printer.prettyPrint(map2);
	
	ac.fromMap(map2);
	auto map3 = ac.toMap();
	qdbg << "\ntoMap from irrelevant:\n" << printer.prettyPrint(map3);
	
	QVariantMap map4;
	
	PoseMapping pm(5);
	pm.setName(0,"Arne");
	pm.setName(1,"Bjarne");
	pm.setName(2,"Peder");
	pm.setName(3,"Kjell");
	pm.setName(4,"Nils");
	pm.setMapping(0,1);
	pm.setMapping(1,3);
	pm.setMapping(2,4);
	pm.setMapping(3,0);
	pm.setMapping(4,2);
	QVariantMap config;
	config["integer"] = 1337;
	config["string"] = "LEET";
	config["bool"] = true;
	map4["controllerName"] = "test_name";
	map4["controllerConfig"] = config;
	map4["poseMapping"] = pm.toMap();
	qdbg << "\nrelevant  map:\n" << printer.prettyPrint(map4);
	
	ac.fromMap(map4);
	auto map5 = ac.toMap();
	qdbg << "\ntoMap from relevant:\n" << printer.prettyPrint(map5);
}


void TestAgentConfig::testAgentConfigStore()
{
	QString fname("TestAgentConfig.store");
	auto file = QSharedPointer<QFile>::create(fname);
	if(file->exists()){
		file->remove();
	}
	QCOMPARE_EQ(file->exists(), false);
	{
		auto acs = QSharedPointer<AgentConfigStore>::create();
		acs->configure(fname);
		acs->activate(true, [=](bool ok){
			qDebug() << "acs.activate():" << (ok?"OK":"FAIL");
			acs->activate(false, [=](bool ok){
				qDebug() << "acs.activate():" << (ok?"OK":"FAIL");
			});
			QCOMPARE_EQ(file->exists(), true);
		});
		qDebug() << "END 1";
	}
	QThread::sleep(1);
	qDebug() << "END 2";
}


OC_TEST_MAIN(test, TestAgentConfig)

