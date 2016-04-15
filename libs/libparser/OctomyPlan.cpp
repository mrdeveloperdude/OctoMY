#include "OctomyPlan.hpp"

OctomyPlan::OctomyPlan()
{

}

//Convenience base class for validating ParseTreeNodes for differnet targets
class Node{
	protected:
		ParseTreeNode &m_node;
		QString m_name;
		QString m_id;
		QVector<QString> m_errors;
		bool m_ok;

	protected:

		void fail(QString msg){
			m_ok=false;
			m_errors.append(msg);
		}

	public:
		Node(ParseTreeNode &node, QString expectedName)
			: m_node(node)
			, m_id(node.hasVariable("id")?node.getVariable("id").toString():"")
			, m_name(node.name())
			, m_ok(true)
		{
			if(expectedName != m_name){
				fail("ERROR: Trying to parse "+expectedName+" with "+m_name);
			}
			else if(""==m_id){
				fail("ERROR: "+m_name+" did not have ID");
			}
		}

		QString id(){
			return m_id;
		}
};



class MemberSpec: public Node
{
	protected:
		QVector<MemberSpec *> subMembers;
	public:
		MemberSpec(ParseTreeNode &node)
			: Node(node, "puppet")
		{
			if(m_ok){

			}
		}
	public:
		void registerSubMember(MemberSpec *ms){
			if(0!=ms){
				subMembers.append(ms);
			}
		}
};




class PuppetSpec: public Node
{
	protected:
		QVector<MemberSpec *> rootMembers;
	public:
		PuppetSpec(ParseTreeNode &node)
			: Node(node, "puppet")
		{
			if(m_ok){

			}
		}
	public:
		void registerRootMember(MemberSpec *ms){
			if(0!=ms){
				rootMembers.append(ms);
			}
		}
};



class AgentSpec: public Node
{
	protected:
		QMap<QString, PuppetSpec *> puppets;
	public:
		AgentSpec(ParseTreeNode &node)
			: Node(node, "spec")
		{
			if(m_ok){
				QVector<ParseTreeNode *> &subnodes=m_node.getChildren();
				for(QVector<ParseTreeNode *>::iterator i=subnodes.begin(), e=subnodes.end();i!=e;++i){
					ParseTreeNode *subnode=(*i);
					if(0!=subnode){
						const QString name=node.name();
						if("puppet"==name){
							PuppetSpec * puppet=new PuppetSpec(node);
							if(0!=puppet){
								puppets.insert(puppet->id(), puppet);
							}
							else{
								fail("ERROR: Could not allocate puppet");
							}
						}
						else if ("spec"==name){
						}
					}
					else{
						fail("ERROR: Found subnode = 0 in agent "+m_id);
					}
				}


			}
		}
};





bool OctomyPlan::fromParseTrees(QVector<ParseTreeNode *> trees)
{
	//errors.clear();
	ParseTreeNode *plan=0;
	PuppetSpec *currentPuppet=0;
	PuppetSpec *currentAgent=0;
	//QStack<MemberSpec *> memberStack;
	for(QVector<ParseTreeNode *>::iterator i=trees.begin(), e=trees.end();i!=e;++i){
		ParseTreeNode *node=(*i);
		if(0!=node){
			const QString name=node->name();
			if("plan"==name){
				if(0==plan){
					plan=node;
				}
				else{
					fail("ERROR: Found more than one plan in trees");
				}
			}
			else if ("spec"==name){
				if(node->hasVariable("id")){
					QString id=node->getVariable("id").toString();
					if(agents.end()==agents.find(id)){
						//AgentSpec *agent=new AgentSpec(id); 						agents.insert(id,agent);
					}
					else{
						fail("ERROR: Found duplicate spec with ID '"+id+"'");
					}
				}
				else{
					fail("ERROR: Found spec missing ID");
				}
			}
			else if ("agent"==name){
				if(0!=currentAgent){
					fail("ERROR: Found agent inside agent");
				}
				else{
					if(node->hasVariable("id")){
						QString id=node->getVariable("id").toString();
						if(agents.end()==agents.find(id)){
							//AgentSpec *agent=new AgentSpec(id);							currentAgent=agent;
						}
						else{
							fail("ERROR: Found duplicate agent with ID '"+id+"'");
						}
					}
					else{
						fail("ERROR: Found spec missing ID");
					}
				}
			}
			else if ("puppet"==name){
				if(0!=currentPuppet){
					fail("ERROR: Found puppet inside puppet");
				}
				else{
					if(node->hasVariable("id")){
						QString id=node->getVariable("id").toString();
						if(agents.end()==agents.find(id)){
							//PuppetSpec *puppet=new PuppetSpec(id); 							currentPuppet=puppet; 							currentAgent->registerPuppet(puppet);
						}
						else{
							fail("ERROR: Found duplicate spec with ID '"+id+"'");
						}
					}
					else{
						fail("ERROR: Found spec missing ID");
					}
				}
			}
			else if ("member"==name){
				//if(MemberSpec){

					//PuppetSpec *puppet=memebersStack.top();
				if(node->hasVariable("id")){
					QString id=node->getVariable("id").toString();
					if(agents.end()==agents.find(id)){
						//AgentSpec *agent=new AgentSpec(id);						agents.insert(id,agent);
					}
					else{
						fail("ERROR: Found duplicate spec with ID '"+id+"'");
					}
				}
				else{
					fail("ERROR: Found spec missing ID");
				}
			}
			else{
				fail("ERROR: Found node with unknown name "+node->name());

			}
		}
		else{
			fail("ERROR: Found node = 0 in trees");
		}
	}
	qDebug()<<"Scanned "<<agents.count()<<" agent specs";
	return false;//errors.count()>0;
}
