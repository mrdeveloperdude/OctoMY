#ifndef OCTOMYPARSERCONTEXT_HPP
#define OCTOMYPARSERCONTEXT_HPP

#include "ParseTreeNode.hpp"
#include <QVector>
#include <QMap>

class AgentNode;
class PlanNode;
class OctomyParseContext
{

private:
	PlanNode *m_plan;
	QVector<QString> m_errors;
	bool m_ok;

public:
	OctomyParseContext();

public:
	bool fromParseTrees(QVector<ParseTreeNode *>trees);
	void fail(QString msg);
	void warn(QString msg);
	void info (QString msg);
	bool isOK();
	QString getErrorText();

};

#endif
// OCTOMYPARSERCONTEXT_HPP
