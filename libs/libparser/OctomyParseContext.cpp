#include "OctomyParseContext.hpp"

OctomyParseContext::OctomyParseContext()
	: m_plan(0)
	, m_ok(true)
{

}

//Convenience base class for validating and parsing ParseTreeNodes into differnet targets (as subclasses)
class Node{
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
			m_plan.fail(msg);
			m_ok=false;
		}

		void warn(QString msg)
		{
			m_plan.warn(msg);
		}

		void info (QString msg)
		{
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
			if(expectedName != m_name){
				fail("Trying to parse "+expectedName+" with "+m_name);
			}
			else if(""==m_id){
				fail(""+m_name+" did not have ID");
			}
			else{
				m_variables=node.getVariables();
			}
		}


		bool hasVariable(QString identifier)
		{
			return m_variables.end()!=m_variables.find(identifier);
		}


		QVariant getVariable(QString identifier)
		{
			if(hasVariable(identifier)){
				return *(m_variables.find(identifier));
			}
			return QVariant();
		}


		void addVariable(QString identifier, QVariant value)
		{
			m_variables.insert(identifier, value);
		}

		bool isOK()
		{
			return m_ok;
		}

		QString id()
		{
			return m_id;
		}
};



class NodeFactory
{
	public:
		Node *	getNodeForParse(ParseTreeNode &node, OctomyParseContext &plan)
		{

		}
};


class DescriptorNode: public Node
{
	protected:

	public:
		DescriptorNode(ParseTreeNode &node, OctomyParseContext &plan, QString name)
			: Node(node, name, plan)
		{
			if(m_ok){

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
			if(m_ok){
				if(""==m_type){
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
			if(m_ok){
				QVector<ParseTreeNode *> &subnodes=m_node.getChildren();
				for(QVector<ParseTreeNode *>::iterator i=subnodes.begin(), e=subnodes.end();i!=e;++i){
					ParseTreeNode *subnode=(*i);
					if(0!=subnode){
						const QString name=node.name();
						if ("member"==name){
							MemberNode *member=new MemberNode(*subnode, m_plan, name);
							if(0!=member){
								registerSubMember(member);
							}
							else{
								fail("Could not allocate submemeber");
							}
						}
						else if ("camera"==name || "serial"==name || "bluetooth"==name || "nfc"==name ){
							DescriptorNode *desc=new DescriptorNode(*subnode, m_plan, name);
							if(0!=desc){
								m_descriptors[name]=desc;
							}
							else{
								fail("Could not allocate descriptor");
							}
						}
						else if ("controller"==name){
							DescriptorNode *controller=new ControllerNode(*subnode, m_plan);
							if(0!=controller){
								m_descriptors[name]=controller;
							}
							else{
								fail("Could not allocate controller");
							}
						}
						else{
							fail("Found unknown node identifier '"+name+"' for member");
						}
					}
					else{
						fail("Found subnode = 0 in agent "+m_id);
					}
				}
			}
		}
	public:
		void registerSubMember(MemberNode *ms){
			if(0!=ms){
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
			if(m_ok){

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
			, m_puppet(0)
		{
			if(m_ok){
				QVector<ParseTreeNode *> &subnodes=m_node.getChildren();
				for(QVector<ParseTreeNode *>::iterator i=subnodes.begin(), e=subnodes.end();i!=e;++i){
					ParseTreeNode *subnode=(*i);
					QVector<QString> unsupported;
					unsupported.append("agent");
					unsupported.append("hub");
					unsupported.append("remote");
					unsupported.append("member");
					unsupported.append("controller");
					unsupported.append("serial");
					if(0!=subnode){
						const QString name=node.name();
						if("puppet"==name){
							if(0!=m_puppet){
								fail("Trying to add second puppet to "+m_id);
							}
							else{
								m_puppet=new PuppetNode(*subnode, m_plan);
								if(0==m_puppet){
									fail("Could not allocate puppet");
								}
							}
						}
						else if(unsupported.contains(name)){
							info("Unsupported name found for spec: "+name);
						}
						else{
							fail("Found unknown node identifier '"+name+"' for spec");
						}
					}
					else{
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

		}
};


class RemoteNode: public SpecNode
{
	protected:

	public:
		RemoteNode(ParseTreeNode &node, OctomyParseContext &plan)
			: SpecNode(node, plan, "remote")
		{

		}
};


class HubNode: public SpecNode
{
	protected:

	public:
		HubNode(ParseTreeNode &node, OctomyParseContext &plan)
			: SpecNode(node, plan, "hub")
		{

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
			if(m_ok){
				QVector<ParseTreeNode *> &subnodes=m_node.getChildren();
				for(QVector<ParseTreeNode *>::iterator i=subnodes.begin(), e=subnodes.end();i!=e;++i){
					ParseTreeNode *subnode=(*i);
					if(0!=subnode){
						const QString name=subnode->name();
						if("agent"==name){
							AgentNode *agent=new AgentNode(*subnode,m_plan);
							if(0!=agent){

							}
							else{
								fail("Could not allocate agent");
							}
						}
						else if ("remote"==name){
							RemoteNode *remote=new RemoteNode(*subnode,m_plan);
							if(0!=remote){

							}
							else{
								fail("Could not allocate remote");
							}

						}
						else if ("hub"==name){
							HubNode *hub=new HubNode(*subnode,m_plan);
							if(0!=hub){

							}
							else{
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

  spec may contain any node with subnodes, it does not need to start on tier
  level. They are stored in the parse context and used as bases for other
  nodes or actual parts of real tiers in a plan.

  nodes have checksums calculated for them so that they can be compared, and
  tier nodes may have their checksums compared during effectuation of new plans
  to verify that the plan's integrity is intact.

  nodes may be compared using a diff tool to find out exactly where they differ


*/



bool OctomyParseContext::fromParseTrees(QVector<ParseTreeNode *> trees)
{
	//errors.clear();
	ParseTreeNode *planTree=0;
	//QStack<MemberSpec *> memberStack;
	for(QVector<ParseTreeNode *>::iterator i=trees.begin(), e=trees.end();i!=e;++i){
		ParseTreeNode *node=(*i);
		if(0!=node){
			const QString name=node->name();
			if("plan"==name){
				if(0==planTree){
					planTree=node;
					m_plan=new PlanNode(*node,*this);
					if(0!=m_plan){
						if(!m_plan->isOK()){
							fail("Could not parse plan");
						}
					}
					else{
						fail("Could not allocate plan");
					}
				}
				else{
					fail("Found more than one plan in trees");
				}
			}
			else if ("spec"==name){
				if(0!=planTree){
					fail("Found spec occurance after plan");
				}
				else{
					SpecNode *spec=new SpecNode(*node, *this);
					if(0!=spec){
						if(!spec->isOK()){
							fail("Could not parse spec");
						}
					}
					else{
						fail("Could not allocate spec");
					}
				}
			}
			else{
				fail("Found unknown identifier in parsetrees");
			}
		}
		else{
			fail("Found node = 0 in trees");
		}
	}
	//	qDebug()<<"Scanned "<<agents.count()<<" agent specs";
	return isOK();
}
