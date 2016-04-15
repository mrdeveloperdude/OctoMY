#include "ParseTreeNode.hpp"





ParseTreeNode::ParseTreeNode(QString name, ParseTreeNode *parent)
	: m_parent(parent)
	, m_name(name)
{

}



QString ParseTreeNode::name()
{
	return m_name;
}

ParseTreeNode *ParseTreeNode::parent()
{
	return m_parent;
}

bool ParseTreeNode::hasVariable(QString identifier)
{
	return m_variables.end()!=m_variables.find(identifier);
}


QVariant ParseTreeNode::getVariable(QString identifier)
{
	if(hasVariable(identifier)){
		return *(m_variables.find(identifier));
	}
	return QVariant();
}

QVector<ParseTreeNode *> &ParseTreeNode::getChildren()
{
	return m_children;
}

void ParseTreeNode::addVariable(QString identifier, QVariant value)
{
	m_variables.insert(identifier, value);
}

void ParseTreeNode::addChild(ParseTreeNode *c)
{
	if(0==c){
		qWarning()<<"ERROR: trying to add nullptr child ";
		return;
	}
	c->m_parent=this;
	m_children.append(c);
}

void ParseTreeNode::clear()
{
	m_variables.clear();
	for(QVector<ParseTreeNode *>::iterator i=m_children.begin(), e=m_children.end();i!=e;++i){
		if(0!=*i){
			(*i)->clear();
		}
	}
	for(QVector<ParseTreeNode *>::iterator i=m_children.begin(), e=m_children.end();i!=e;++i){
		if(0!=*i){
			delete *i;
		}
	}
	m_children.clear();
}

QString ParseTreeNode::toString(QString space)
{
	QString out;
	QString mySpace=space+"\t";
	out+=space+m_name+" {\n";
	for(QMap<QString, QVariant>::iterator i=m_variables.begin(), e=m_variables.end();i!=e;++i){
		out+=mySpace+i.key()+" = "+i.value().value<QString>()+"\n";
	}
	for(QVector<ParseTreeNode *>::iterator i=m_children.begin(), e=m_children.end();i!=e;++i){
		if(0!=*i){
			out+=(*i)->toString(mySpace);
		}
	}
	out+=space+"}\n";
	return out;
}
