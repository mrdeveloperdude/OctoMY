#include "TestZooComms.hpp"
#include "zoo/ZooServer.hpp"
#include "zoo/ZooClient.hpp"

#include <QSignalSpy>

#include <QProcessEnvironment>
#include <QCommandLineParser>

// YOU NEED THIS: http://doc.qt.io/qt-5/qtest.html
void TestZooComms::test(){
	QString port="8123";

	QProcessEnvironment env=QProcessEnvironment::systemEnvironment();

	QCommandLineParser opts;

	opts.setApplicationDescription("Robust real-time communication and control software for robots");
	opts.addHelpOption();

	ZooServer server(opts, env, nullptr);
	server.start(port);

	ZooClient *client=new ZooClient(this);


	QSignalSpy spyPut(client, SIGNAL(putNodeDone()));
	QSignalSpy spyGet(client, SIGNAL(getNodeDone()));


	client->setURL("http://127.0.0.1:"+port+"/api");
	QString key1="DC97D8A6C744EC29B3FD2C8EC0E9CA56760BA98A";
	QString key2="XC97D8A6C744EC29B3FD2C8EC0E9CA56760BA98A";
	QByteArray val1="lolbob-1";
	QByteArray  val2="lolbob-2";
	client->putNodeCrumb(key1,val1);
	if(!spyPut.wait(1000)){
		qWarning()<<"spyPut-1 wait timed out";
	}
	client->putNodeCrumb(key2,val2);
	if(!spyPut.wait(1000)){
		qWarning()<<"spyPut-2 wait timed out";
	}
	TVariantMapHandler gotNodeCrumb1=[this, val1](bool ok, QVariantMap map){
		qDebug()<<"gotNode["<<ok<<"]: "<<map<<" vs. "<<val1;
	};
	TVariantMapHandler gotNodeCrumb2=[this, val2](bool ok, QVariantMap map){
		qDebug()<<"gotNode["<<ok<<"]: "<<map<<" vs. "<<val2;
	};

	client->getNodeCrumb(key1, gotNodeCrumb1);
	if(!spyGet.wait(1000)){
		qWarning()<<"spyGet-1 wait timed out";
	}
	client->getNodeCrumb(key2, gotNodeCrumb2);
	if(!spyGet.wait(1000)){
		qWarning()<<"spyGet-2 wait timed out";
	}

	QCOMPARE(spyPut.count(), 2);
	QCOMPARE(spyGet.count(), 2);


}



QTEST_MAIN(TestZooComms)
