#include "OctomyParseContext.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

OctomyParseContext::OctomyParseContext()
	: m_plan(nullptr)
	, m_ok(true)
{
	OC_METHODGATE();
}

void OctomyParseContext::fail(QString msg)
{
	OC_METHODGATE();
	qWarning()<<"E:" <<msg;
	m_ok=false;
	m_errors.append("ERROR:   " + msg);
}

void OctomyParseContext::warn(QString msg)
{
	OC_METHODGATE();
	qWarning()<<"W:" <<msg;
	m_errors.append("WARNING: " + msg);
}

void OctomyParseContext::info (QString msg)
{
	OC_METHODGATE();
	qDebug()<<"I:" <<msg;
	m_errors.append("INFO   : " + msg);
}


bool OctomyParseContext::isOK()
{
	OC_METHODGATE();
	return m_ok;
}

QString OctomyParseContext::getErrorText()
{
	OC_METHODGATE();
	QString out="";
	for(QVector<QString>::iterator i=m_errors.begin(), e=m_errors.end(); e!=i; ++i) {
		out+=(*i)+"\n";
	}
	return out;
}

// Convenience base class for validating and parsing ParseTreeNodes into differnet targets (as subclasses)
class Node
{
protected:
	ParseTreeNode &m_node;
	OctomyParseContext &m_plan;
	QString m_name;
	QString m_id;
	bool m_ok;
	QMap<QString, QVariant> m_variables;


protected:

	void fail(QString msg)
	{
		OC_METHODGATE();
		m_plan.fail(msg);
		m_ok=false;
	}

	void warn(QString msg)
	{
		OC_METHODGATE();
		m_plan.warn(msg);
	}

	void info (QString msg)
	{
		OC_METHODGATE();
		m_plan.info(msg);
	}

public:
	Node(ParseTreeNode &node, QString expectedName, OctomyParseContext &plan)
		: m_node(node)
		, m_plan(plan)
		, m_name(node.name())
		, m_id(node.hasVariable("id")?node.getVariable("id").toString():"")
		, m_ok(true)
	{
		OC_METHODGATE();
		if(expectedName != m_name) {
			fail("Trying to parse "+expectedName+" with "+m_name);
		} else if(""==m_id) {
			fail(""+m_name+" did not have ID");
		} else {
			m_variables=node.getVariables();
		}
	}


	bool hasVariable(QString identifier)
	{
		OC_METHODGATE();
		return m_variables.end()!=m_variables.find(identifier);
	}


	QVariant getVariable(QString identifier)
	{
		OC_METHODGATE();
		if(hasVariable(identifier)) {
			return *(m_variables.find(identifier));
		}
		return QVariant();
	}


	void addVariable(QString identifier, QVariant value)
	{
		OC_METHODGATE();
		m_variables.insert(identifier, value);
	}

	bool isOK()
	{
		OC_METHODGATE();
		return m_ok;
	}

	QString id()
	{
		OC_METHODGATE();
		return m_id;
	}
};



class NodeFactory
{
public:
	Node *	getNodeForParse(ParseTreeNode &node, OctomyParseContext &plan)
	{
		OC_METHODGATE();
		Q_UNUSED(node);
		Q_UNUSED(plan);
		return nullptr;
// TODO: implement
	}
};


class DescriptorNode: public Node
{
protected:

public:
	DescriptorNode(ParseTreeNode &node, OctomyParseContext &plan, QString name)
		: Node(node, name, plan)
	{
		OC_METHODGATE();
		if(m_ok) {

		}
	}
public:

};





class ControllerNode: public DescriptorNode
{
protected:

	QString m_type;
public:
	ControllerNode(ParseTreeNode &node, OctomyParseContext &plan)
		: DescriptorNode(node, plan, "controller")
		, m_type(node.hasVariable("type")?node.getVariable("type").toString():"")
	{
		OC_METHODGATE();
		if(m_ok) {
			if(""==m_type) {
				fail(""+m_name+" did not have a type");
			}

		}
	}
public:

};



class MemberNode: public Node
{
protected:
	QVector<MemberNode *> m_members;
	QMap<QString, DescriptorNode *> m_descriptors;
public:
	MemberNode(ParseTreeNode &node, OctomyParseContext &plan,  QString name="member")
		: Node(node, name, plan)
	{
		OC_METHODGATE();
		if(m_ok) {
			QVector<ParseTreeNode *> &subnodes=m_node.getChildren();
			for(QVector<ParseTreeNode *>::iterator i=subnodes.begin(), e=subnodes.end(); i!=e; ++i) {
				ParseTreeNode *subnode=(*i);
				if(nullptr != subnode) {
					const QString name=node.name();
					if ("member"==name) {
						MemberNode *member=OC_NEW MemberNode(*subnode, m_plan, name);
						if(nullptr != member) {
							registerSubMember(member);
						} else {
							fail("Could not allocate submemeber");
						}
					} else if ("camera"==name || "serial"==name || "bluetooth"==name || "nfc"==name ) {
						DescriptorNode *desc=OC_NEW DescriptorNode(*subnode, m_plan, name);
						if(nullptr != desc) {
							m_descriptors[name]=desc;
						} else {
							fail("Could not allocate descriptor");
						}
					} else if ("controller"==name) {
						DescriptorNode *controller=OC_NEW ControllerNode(*subnode, m_plan);
						if(nullptr != controller) {
							m_descriptors[name]=controller;
						} else {
							fail("Could not allocate controller");
						}
					} else {
						fail("Found unknown node identifier '"+name+"' for member");
					}
				} else {
					fail("Found subnode = 0 in agent "+m_id);
				}
			}
		}
	}
public:
	void registerSubMember(MemberNode *ms)
	{
		OC_METHODGATE();
		if(nullptr!=ms) {
			m_members.append(ms);
		}
	}
};




class PuppetNode: public MemberNode
{
protected:

public:
	PuppetNode(ParseTreeNode &node, OctomyParseContext &plan, QString name="puppet")
		: MemberNode(node, plan, name)
	{
		OC_METHODGATE();
		if(m_ok) {

		}
	}
public:

};





class SpecNode: public Node
{
protected:

	PuppetNode *m_puppet;


public:
	SpecNode(ParseTreeNode &node, OctomyParseContext &plan, QString name="spec")
		: Node(node, name, plan)
		, m_puppet(nullptr)
	{
		OC_METHODGATE();
		if(m_ok) {
			QVector<ParseTreeNode *> &subnodes=m_node.getChildren();
			for(QVector<ParseTreeNode *>::iterator i=subnodes.begin(), e=subnodes.end(); i!=e; ++i) {
				ParseTreeNode *subnode=(*i);
				QVector<QString> unsupported;
				unsupported.append("agent");
				unsupported.append("hub");
				unsupported.append("remote");
				unsupported.append("member");
				unsupported.append("controller");
				unsupported.append("serial");
				if(nullptr != subnode) {
					const QString name=node.name();
					if("puppet"==name) {
						if(nullptr != m_puppet) {
							fail("Trying to add second puppet to "+m_id);
						} else {
							m_puppet=OC_NEW PuppetNode(*subnode, m_plan);
							if(nullptr == m_puppet) {
								fail("Could not allocate puppet");
							}
						}
					} else if(unsupported.contains(name)) {
						info("Unsupported name found for spec: "+name);
					} else {
						fail("Found unknown node identifier '"+name+"' for spec");
					}
				} else {
					fail("Found subnode = 0 in "+m_name+" with id '"+m_id+"'");
				}
			}
		}
	}
};



class AgentNode: public SpecNode
{
protected:

public:
	AgentNode(ParseTreeNode &node, OctomyParseContext &plan)
		: SpecNode(node, plan, "agent")
	{
		OC_METHODGATE();
	}
};


class RemoteNode: public SpecNode
{
protected:

public:
	RemoteNode(ParseTreeNode &node, OctomyParseContext &plan)
		: SpecNode(node, plan, "remote")
	{
		OC_METHODGATE();
	}
};


class HubNode: public SpecNode
{
protected:

public:
	HubNode(ParseTreeNode &node, OctomyParseContext &plan)
		: SpecNode(node, plan, "hub")
	{
		OC_METHODGATE();
	}
};


class PlanNode: public Node
{
protected:

	QMap<QString, AgentNode *> m_agents;
	QMap<QString, RemoteNode *> m_remotes;
	QMap<QString, HubNode *> m_hubs;

public:
	PlanNode(ParseTreeNode &node, OctomyParseContext &plan)
		: Node(node, "plan", plan)
	{
		OC_METHODGATE();
		if(m_ok) {
			QVector<ParseTreeNode *> &subnodes=m_node.getChildren();
			for(QVector<ParseTreeNode *>::iterator i=subnodes.begin(), e=subnodes.end(); i!=e; ++i) {
				ParseTreeNode *subnode=(*i);
				if(nullptr != subnode) {
					const QString name=subnode->name();
					if("agent"==name) {
						AgentNode *agent=OC_NEW AgentNode(*subnode,m_plan);
						if(nullptr != agent) {

						} else {
							fail("Could not allocate agent");
						}
					} else if ("remote"==name) {
						RemoteNode *remote=OC_NEW RemoteNode(*subnode,m_plan);
						if(nullptr != remote) {

						} else {
							fail("Could not allocate remote");
						}

					} else if ("hub"==name) {
						HubNode *hub=OC_NEW HubNode(*subnode,m_plan);
						if(nullptr != hub) {

						} else {
							fail("Could not allocate hub");
						}

					}
				}
			}
		}
	}
public:

};

/*

  Notes for octomy plan

  spec may contain any node with subnodes, it does not need to start on node
  level. They are stored in the parse context and used as bases for other
  nodes or actual parts of real nodes in a plan.

  nodes have checksums calculated for them so that they can be compared, and
  nodes may have their checksums compared during effectuation of new plans
  to verify that the plan's integrity is intact.

  nodes may be compared using a diff tool to find out exactly where they differ


*/



bool OctomyParseContext::fromParseTrees(QVector<ParseTreeNode *> trees)
{
	//errors.clear();
	ParseTreeNode *planTree=nullptr;
	//QStack<MemberSpec *> memberStack;
	for(QVector<ParseTreeNode *>::iterator i=trees.begin(), e=trees.end(); i!=e; ++i) {
		ParseTreeNode *node=(*i);
		if(nullptr != node) {
			const QString name=node->name();
			if("plan"==name) {
				if(nullptr == planTree) {
					planTree=node;
					m_plan=OC_NEW PlanNode(*node,*this);
					if(nullptr != m_plan) {
						if(!m_plan->isOK()) {
							fail("Could not parse plan");
						}
					} else {
						fail("Could not allocate plan");
					}
				} else {
					fail("Found more than one plan in trees");
				}
			} else if ("spec"==name) {
				if(nullptr != planTree) {
					fail("Found spec occurance after plan");
				} else {
					SpecNode *spec=OC_NEW SpecNode(*node, *this);
					if(nullptr != spec) {
						if(!spec->isOK()) {
							fail("Could not parse spec");
						}
					} else {
						fail("Could not allocate spec");
					}
				}
			} else {
				fail("Found unknown identifier in parsetrees");
			}
		} else {
			fail("Found node = 0 in trees");
		}
	}
	//	qDebug()<<"Scanned "<<agents.count()<<" agent specs";
	return isOK();
}
