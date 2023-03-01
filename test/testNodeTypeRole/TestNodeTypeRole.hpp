#ifndef TESTNODETYPEROLE_HPP
#define TESTNODETYPEROLE_HPP

#include "test/Common.hpp"

class TestNodeTypeRole:public QObject
{
	Q_OBJECT

private slots:
	void testNodeTypeQDebugStreamOperator();
	void testNodeTypeToString();
	void testNodeTypeFromString();
	void testDefaultPortForNodeType();
	void testDefaultColorForNodeType();

	void testNodeRoleQDebugStreamOperator();
	void testNodeRoleNodeRoleToString();
	void testNodeRoleNodeRoleFromString();

};

#endif
// TESTNODETYPEROLE_HPP
