#include "NodeFactory.hpp"


#include "agent/Agent.hpp"
#include "hub/Hub.hpp"
#include "remote/Remote.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QProcess>
#include <QRandomGenerator>
#include <QThread>

#include <csignal>
#include <fcntl.h>
#include <private/qcore_unix_p.h>
#include <private/qprocess_p.h>
#include <unistd.h>

NodeFactory::NodeFactory(const QString &prefix)
	: mPrefix(prefix)
{
	
}



QString NodeFactory::startProcess(QString base, QObject *parent){
	OC_METHODGATE();
	const QString program = QFileInfo(QCoreApplication::applicationFilePath()).absolutePath()+"/../"+base+"/"+base;
	qDebug()<<"Starting process: "<<program;
	QStringList arguments;
	//arguments << "-style" << "fusion";
	QProcess *proc = OC_NEW QProcess(parent);
	proc->setProcessChannelMode(QProcess::ForwardedChannels);
	proc->start(program, arguments);
	//TODO: make async
	proc->waitForStarted();
	if(QProcess::Running!=proc->state()) {
		const auto err = proc->errorString();
		qDebug()<<"ERROR could not start: "<<err;
		proc->deleteLater();
		proc = nullptr;
		return err;
	} else {
		qDebug()<<"A-OK";
	}
	return QString();
}


QString randomID()
{
	QString randomHex;
	for (int i = 0; i < 5; ++i) {
		// Generate a random number between 0 and 15 (0xF) and convert it to uppercase hex
		int randomValue = QRandomGenerator::global()->bounded(16);
		randomHex.append(QString::number(randomValue, 16).toUpper());
	}
	return randomHex;
}


void nodeFactory(NodeType nodeType, const QString &name){
	QTimer::singleShot(0, [=]() {
		switch(nodeType){
		case TYPE_AGENT: {
			auto node = QSharedPointer<AppLauncher<Agent> >::create();
			if(!node.isNull()) {
				node->configure(name);
				const auto ret = node->run();
				qWarning() << "Launcher" << name << "returned" << ret;
				node.clear();
			} else {
				qWarning() << "ERROR: Could not allocate launcher for " << name;
			}
		}break;
		case TYPE_REMOTE:{
			auto node = QSharedPointer<AppLauncher<Remote> >::create();
			if(!node.isNull()) {
				node->configure(name);
				const auto ret = node->run();
				qWarning() << "Launcher" << name << "returned" << ret;
				node.clear();
			} else {
				qWarning() << "ERROR: Could not allocate launcher for " << name;
			}	
		}break;
		case TYPE_HUB: {
			auto node = QSharedPointer<AppLauncher<Hub> >::create();
			if(!node.isNull()) {
				node->configure(name);
				const auto ret = node->run();
				qWarning() << "Launcher" << name << "returned" << ret;
				node.clear();
			} else {
				qWarning() << "ERROR: Could not allocate launcher for " << name;
			}
		}break;
		case TYPE_ZOO: {}break;
		case TYPE_UNKNOWN: {}break;
		default:{
			qCritical() << nodeType <<" Unknown node type";
		}
		}
	});
}


QString NodeFactory::nodeName(NodeType nodeType){
	int index{0};
	switch(nodeType){
	case TYPE_AGENT: index = mAgentNodeIndex++; break;
	case TYPE_REMOTE: index = mRemoteNodeIndex++; break;
	case TYPE_HUB: index = mHubNodeIndex++; break;
	default:
		break;
	}
	const auto name = QString("%1%2-%3").arg(mPrefix).arg(nodeTypeToString(nodeType)).arg(index);
	return name;
}


void ForkThread::run() {
	qDebug() << "forkThred run.";
	QEventLoop eventLoop;
	nodeFactory(nodeType, name);
	// Start the event loop
	eventLoop.exec();
	
	// This line will be reached only when the event loop exits
	qDebug() << "forkThred Event loop finished.";
}



bool NodeFactory::forkNodeEventloop(NodeType nodeType){
	nodeFactory(nodeType, nodeName(nodeType));
	return true;
}


bool NodeFactory::forkNodeThread(NodeType nodeType, QObject *parent){
	const auto name = nodeName(nodeType);
	auto const thread = OC_NEW ForkThread(nodeType, name, parent);
	thread->start();
	return thread->isRunning();
}

bool NodeFactory::forkNodeProcess(NodeType nodeType, bool detatchProcess, bool detatchLog){
	OC_METHODGATE();
	auto id = randomID();
	auto pid = fork();
	if (pid < 0) {
		// Error occurred during fork
		qCritical() << id <<" Fork failed:" << strerror(errno);
		return false;
	} else if (pid == 0) {
		id = randomID();
		qDebug() << id <<" Child PID:" << getpid();
		if(detatchProcess){
			// Detatch child from parent
			if (setsid() < 0) {
				qCritical() << id <<" Failed to detatch from parent process";
				_exit(1);
			}
			if(detatchLog){
				// Redirect standard input, output, and error to /dev/null
				int fd = open("/dev/null", O_RDWR);
				if (fd != -1) {
					dup2(fd, STDIN_FILENO);
					dup2(fd, STDOUT_FILENO);
					dup2(fd, STDERR_FILENO);
					if (fd > STDERR_FILENO) {
						::close(fd);
					}
				}
			}
			// Shut down eventloop for pre-fork environment...
			//qApp->exit(0);
			qDebug() << id <<" Child process soft exit";
			QCoreApplication::quit();
			qDebug() << id <<" Child process complete pending events";
			QCoreApplication::processEvents(QEventLoop::AllEvents);
			qDebug() << id <<" Child process hard exit";
			QCoreApplication::exit(0);
			qDebug() << id <<" Child process eventloop was quit!";
		}
		// Before starting new app fresh
		nodeFactory(nodeType, nodeName(nodeType));
	} else {
		// Parent process block
		qDebug() << id <<" Parent PID:" << getpid() << "Child PID:" << pid;
	}
	return true;
}
