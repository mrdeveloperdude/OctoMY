#ifndef OCTOMYPLAN_HPP
#define OCTOMYPLAN_HPP

#include "ParseTreeNode.hpp"
#include <QVector>
#include <QMap>

class AgentNode;

class OctomyPlan
{

	private:
		QMap<QString, AgentNode *> agents;

	public:
		OctomyPlan();

	public:
		bool fromParseTrees(QVector<ParseTreeNode *>trees);

		void fail(QString){

		}

};

#endif // OCTOMYPLAN_HPP
