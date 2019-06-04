#include "TestNodeTypeRole.hpp"

#include "node/NodeRole.hpp"
#include "node/NodeType.hpp"
#include "app/Constants.hpp"

void TestNodeTypeRole::testNodeTypeQDebugStreamOperator()
{
	////////////////////////////////////////////////////////////////////////////
	// const QDebug &operator<<(QDebug &d, const NodeType nodeType);
	// TODO: Implement test for this
}


void TestNodeTypeRole::testNodeTypeToString()
{
	////////////////////////////////////////////////////////////////////////////
	// QString nodeTypeToString(NodeType nodeType);
	QCOMPARE(nodeTypeToString(TYPE_AGENT), Constants::OC_NODE_TYPE_AGENT_NAME);
	QCOMPARE(nodeTypeToString(TYPE_REMOTE), Constants::OC_NODE_TYPE_REMOTE_NAME);
	QCOMPARE(nodeTypeToString(TYPE_HUB), Constants::OC_NODE_TYPE_HUB_NAME);
	QCOMPARE(nodeTypeToString(TYPE_ZOO), Constants::OC_NODE_TYPE_ZOO_NAME);
	QCOMPARE(nodeTypeToString(TYPE_UNKNOWN), Constants::OC_NODE_TYPE_UNKNOWN_NAME);
	QCOMPARE(nodeTypeToString(static_cast<NodeType>(TYPE_UNKNOWN +1) ), Constants::OC_NODE_TYPE_UNKNOWN_NAME);
}


void TestNodeTypeRole::testNodeTypeFromString()
{
	////////////////////////////////////////////////////////////////////////////
	// NodeType nodeTypeFromString(QString string);
	QCOMPARE(nodeTypeFromString(Constants::OC_NODE_TYPE_AGENT_NAME), TYPE_AGENT);
	QCOMPARE(nodeTypeFromString(Constants::OC_NODE_TYPE_REMOTE_NAME), TYPE_REMOTE);
	QCOMPARE(nodeTypeFromString(Constants::OC_NODE_TYPE_HUB_NAME), TYPE_HUB);
	QCOMPARE(nodeTypeFromString(Constants::OC_NODE_TYPE_ZOO_NAME), TYPE_ZOO);
	QCOMPARE(nodeTypeFromString(Constants::OC_NODE_TYPE_UNKNOWN_NAME), TYPE_UNKNOWN);
	QCOMPARE(nodeTypeFromString("Bollocks"), TYPE_UNKNOWN);
}


void TestNodeTypeRole::testDefaultPortForNodeType()
{
	////////////////////////////////////////////////////////////////////////////
	// quint16 defaultPortForNodeType(NodeType nodeType);
	QCOMPARE(defaultPortForNodeType(TYPE_AGENT), Constants::OCTOMY_UDP_DEFAULT_PORT_AGENT);
	QCOMPARE(defaultPortForNodeType(TYPE_REMOTE), Constants::OCTOMY_UDP_DEFAULT_PORT_REMOTE);
	QCOMPARE(defaultPortForNodeType(TYPE_HUB), Constants::OCTOMY_UDP_DEFAULT_PORT_HUB);
	QCOMPARE(defaultPortForNodeType(TYPE_ZOO), Constants::OCTOMY_UDP_DEFAULT_PORT_ZOO);
	QCOMPARE(defaultPortForNodeType(TYPE_UNKNOWN), Constants::OCTOMY_UDP_DEFAULT_PORT_UNKNOWN);
	QCOMPARE(defaultPortForNodeType(static_cast<NodeType>(TYPE_UNKNOWN +1) ), Constants::OCTOMY_UDP_DEFAULT_PORT_UNKNOWN);
}


void TestNodeTypeRole::testDefaultColorForNodeType()
{
	////////////////////////////////////////////////////////////////////////////
	// QColor defaultColorForNodeType(NodeType nodeType);
	QCOMPARE(defaultColorForNodeType(TYPE_AGENT), Constants::OC_NODE_TYPE_AGENT_COLOR);
	QCOMPARE(defaultColorForNodeType(TYPE_REMOTE), Constants::OC_NODE_TYPE_REMOTE_COLOR);
	QCOMPARE(defaultColorForNodeType(TYPE_HUB), Constants::OC_NODE_TYPE_HUB_COLOR);
	QCOMPARE(defaultColorForNodeType(TYPE_ZOO), Constants::OC_NODE_TYPE_ZOO_COLOR);
	QCOMPARE(defaultColorForNodeType(TYPE_UNKNOWN), Constants::OC_NODE_TYPE_UNKNOWN_COLOR);
	QCOMPARE(defaultColorForNodeType(static_cast<NodeType>(TYPE_UNKNOWN +1) ), Constants::OC_NODE_TYPE_UNKNOWN_COLOR);
}


void TestNodeTypeRole::testNodeRoleQDebugStreamOperator()

{
	////////////////////////////////////////////////////////////////////////////
	// const QDebug &operator<<(QDebug &d, const NodeRole &role);
	// TODO: Implement this
}


void TestNodeTypeRole::testNodeRoleNodeRoleToString()
{
	////////////////////////////////////////////////////////////////////////////
	// QString nodeRoleToString(NodeRole s);
	QCOMPARE(nodeRoleToString(ROLE_AGENT), Constants::OC_NODE_ROLE_AGENT_NAME);
	QCOMPARE(nodeRoleToString(ROLE_CONTROL), Constants::OC_NODE_ROLE_CONTROL_NAME);
	QCOMPARE(nodeRoleToString(ROLE_UNKNOWN), Constants::OC_NODE_ROLE_UNKNOWN_NAME);
	QCOMPARE(nodeRoleToString(static_cast<NodeRole>(ROLE_UNKNOWN +1)), Constants::OC_NODE_ROLE_UNKNOWN_NAME);
}


void TestNodeTypeRole::testNodeRoleNodeRoleFromString()
{
	////////////////////////////////////////////////////////////////////////////
	// NodeRole nodeRoleFromString(QString s);
	QCOMPARE(nodeRoleFromString(Constants::OC_NODE_ROLE_AGENT_NAME), ROLE_AGENT);
	QCOMPARE(nodeRoleFromString(Constants::OC_NODE_ROLE_CONTROL_NAME), ROLE_CONTROL);
	QCOMPARE(nodeRoleFromString(Constants::OC_NODE_ROLE_UNKNOWN_NAME), ROLE_UNKNOWN);
	QCOMPARE(nodeRoleFromString("Bollocks"), ROLE_UNKNOWN);

}

OC_TEST_MAIN(test, TestNodeTypeRole)

