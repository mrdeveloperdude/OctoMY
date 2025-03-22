#ifndef NODEFACTORY_HPP
#define NODEFACTORY_HPP


#include "uptime/SharedPointerWrapper.hpp"
#include <QThread>
#include <QTimer>
#include <QWidget>

#include "node/NodeType.hpp"



class ForkThread : public QThread {
	Q_OBJECT
private:
	NodeType nodeType;
	QString name;

public:
	ForkThread(const NodeType nodeType, const QString &name, QObject *parent = nullptr)
		: QThread(parent)
		, nodeType(nodeType)
		, name(name)
	{}
	
protected:
	void run() override;
};




class NodeFactory
{
private:
	QString mPrefix;
	int mAgentNodeIndex{1};
	int mRemoteNodeIndex{1};
	int mHubNodeIndex{1};
public:
	NodeFactory(const QString &prefix=QString());

public:
	QString nodeName(NodeType nodeType);
	QString startProcess(QString base, QObject *parent = nullptr);
	bool forkNodeEventloop(NodeType nodeType);
	bool forkNodeThread(NodeType nodeType, QObject *parent = nullptr);
	bool forkNodeProcess(NodeType nodeType, bool detatchProcess = true, bool detatchLog = false);

};

#endif // NODEFACTORY_HPP
