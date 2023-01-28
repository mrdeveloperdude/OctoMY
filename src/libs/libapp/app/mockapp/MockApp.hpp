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
	virtual ~MockApp() Q_DECL_OVERRIDE;

private:
	void handleMock(QString event);

	// Node interface
public:
	// Called by Node::appConfigure();
	void nodeConfigure() Q_DECL_OVERRIDE;

	// Called by Node::appActivate()
	void nodeActivate(const bool on) Q_DECL_OVERRIDE;

	// Called by Node::appWindow()
	QSharedPointer<NodeWindow> nodeWindow() Q_DECL_OVERRIDE;

	// Provide role
	NodeRole nodeRole() Q_DECL_OVERRIDE;

	// Provide type
	NodeType nodeType() Q_DECL_OVERRIDE;
	
	
	// QEnableSharedFromThis<Agent> interface
public:
	QSharedPointer<Node> sharedThis() Q_DECL_OVERRIDE;


signals:
	// Emitted when this mock app experiences any change
	void mockEvent(QString event);

};


#endif
// MOCKAPP_HPP
