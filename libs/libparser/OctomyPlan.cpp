#include "OctomyPlan.hpp"

OctomyPlan::OctomyPlan()
{

}

//Convenience base class for validating and parsing ParseTreeNodes into differnet targets (as subclasses)
class Node{
	protected:
		ParseTreeNode &m_node;
		OctomyPlan &m_plan;
		QString m_name;
		QString m_id;
		QVector<QString> m_errors;
		bool m_ok;
		QMap<QString, QVariant> m_variables;


	protected:

		void fail(QString msg){
			m_ok=false;
			m_errors.append(msg);
		}

	public:
		Node(ParseTreeNode &node, QString expectedName, OctomyPlan &plan)
			: m_node(node)
			, m_plan(plan)
			, m_name(node.name())
			, m_id(node.hasVariable("id")?node.getVariable("id").toString():"")
			, m_ok(true)
		{
			if(expectedName != m_name){
				fail("ERROR: Trying to parse "+expectedName+" with "+m_name);
			}
			else if(""==m_id){
				fail("ERROR: "+m_name+" did not have ID");
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


		QString id(){
			return m_id;
		}
};




class DescriptorNode: public Node
{
	protected:

	public:
		DescriptorNode(ParseTreeNode &node, OctomyPlan &plan, QString name)
			: Node(node, name, plan)
		{
			if(m_ok){

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
		MemberNode(ParseTreeNode &node, OctomyPlan &plan,  QString name="member")
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
								fail("ERROR: Could not allocate submemeber");
							}
						}
						else if ("camera"==name || "controller"==name || "serial"==name || "bluetooth"==name || "nfc"==name ){
							DescriptorNode *desc=new DescriptorNode(*subnode, m_plan, name);
							if(0!=desc){
								m_descriptors[name]=desc;
							}
							else{
								fail("ERROR: Could not allocate descriptor");
							}
						}
						else{
							fail("ERROR: Found unknown node identifier '"+name+"'");
						}
					}
					else{
						fail("ERROR: Found subnode = 0 in agent "+m_id);
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
		PuppetNode(ParseTreeNode &node, OctomyPlan &plan, QString name="puppet")
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
		SpecNode(ParseTreeNode &node, OctomyPlan &plan, QString name="spec")
			: Node(node, name, plan)
			, m_puppet(0)
		{
			if(m_ok){
				QVector<ParseTreeNode *> &subnodes=m_node.getChildren();
				for(QVector<ParseTreeNode *>::iterator i=subnodes.begin(), e=subnodes.end();i!=e;++i){
					ParseTreeNode *subnode=(*i);
					if(0!=subnode){
						const QString name=node.name();
						if("puppet"==name){
							if(0!=m_puppet){
								fail("ERROR: Trying to add second puppet to "+m_id);
							}
							else{
								m_puppet=new PuppetNode(*subnode, m_plan);
								if(0==m_puppet){
									fail("ERROR: Could not allocate puppet");
								}
							}
						}
						else{
							fail("ERROR: Found unknown node identifier '"+name+"'");
						}
					}
					else{
						fail("ERROR: Found subnode = 0 in "+m_name+" with id '"+m_id+"'");
					}
				}
			}
		}
};



class AgentNode: public SpecNode
{
	protected:

	public:
		AgentNode(ParseTreeNode &node, OctomyPlan &plan)
			: SpecNode(node, plan, "agent")
		{

		}
};


class RemoteNode: public SpecNode
{
	protected:

	public:
		RemoteNode(ParseTreeNode &node, OctomyPlan &plan)
			: SpecNode(node, plan, "remote")
		{

		}
};


class HubNode: public SpecNode
{
	protected:

	public:
		HubNode(ParseTreeNode &node, OctomyPlan &plan)
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
		PlanNode(ParseTreeNode &node, OctomyPlan &plan)
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
								fail("ERROR: Could not allocate agent");
							}
						}
						else if ("remote"==name){
							RemoteNode *remote=new RemoteNode(*subnode,m_plan);
							if(0!=remote){

							}
							else{
								fail("ERROR: Could not allocate remote");
							}

						}
						else if ("hub"==name){
							HubNode *hub=new HubNode(*subnode,m_plan);
							if(0!=hub){

							}
							else{
								fail("ERROR: Could not allocate hub");
							}

						}
					}
				}
			}
		}
	public:

};



bool OctomyPlan::fromParseTrees(QVector<ParseTreeNode *> trees)
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
					//Plan
				}
				else{
					fail("ERROR: Found more than one plan in trees");
				}
			}
			else if ("spec"==name){
				if(0!=planTree){
					fail("ERROR: Found spec occurance after plan");
				}
				else{
					SpecNode *spec=new SpecNode(*node, *this);
					if(0!=spec){

					}
					else{
						fail("ERROR: Could not allocate spec");
					}
				}
			}
			else{
				fail("ERROR: Found unknown identifier in parsetrees");
			}
		}
		else{
			fail("ERROR: Found node = 0 in trees");
		}
	}
	qDebug()<<"Scanned "<<agents.count()<<" agent specs";
	return false;//errors.count()>0;
}
