#ifndef MOCKAPP_HPP
#define MOCKAPP_HPP

#include "node/Node.hpp"
#include "node/NodeRole.hpp"
#include "node/NodeType.hpp"

#include <QObject>


class CommsSession;
		
class MockApp: public Node//, public QEnableSharedFromThis<Agent>
{
	Q_OBJECT

private:
	QSharedPointer<NodeWindow> mWindow;
	NodeRole mNodeRole{ROLE_UNKNOWN};
	NodeType mNodeType{TYPE_UNKNOWN};

public:
	explicit MockApp();
	virtual ~MockApp() override;

private:
	void handleMock(QString event);

	// Node interface
public:
	// Called by Node::appConfigure();
	void nodeConfigure() override;

	// Called by Node::appActivate()
	void nodeActivate(const bool on) override;

	// Called by Node::appWindow()
	QSharedPointer<NodeWindow> nodeWindow() override;

	// Provide role
	NodeRole nodeRole() override;

	// Provide type
	NodeType nodeType() override;
	
	
	// QEnableSharedFromThis<Agent> interface
public:
	QSharedPointer<Node> sharedThis() override;


signals:
	// Emitted when this mock app experiences any change
	void mockEvent(QString event);

};


#endif
// MOCKAPP_HPP
