#ifndef PARSETREENODE_HPP
#define PARSETREENODE_HPP

#include <QString>
#include <QMap>
#include <QVector>
#include <QVariant>
#include <QDebug>

class ParseTreeNode{

	private:
		ParseTreeNode *m_parent;
		QString m_name;
		QMap<QString, QVariant> m_variables;
		QVector<ParseTreeNode *> m_children;


	public:
		ParseTreeNode(QString name="", ParseTreeNode *parent=0);
	public:

		QString name();
		ParseTreeNode *parent();
		bool hasVariable(QString identifier);
		QVariant getVariable(QString identifier);
		QVector<ParseTreeNode *> &getChildren();
		void addVariable(QString identifier, QVariant value);
		void addChild(ParseTreeNode *c);
		void clear();
		QString toString(QString space="");
};


#endif // PARSETREENODE_HPP
