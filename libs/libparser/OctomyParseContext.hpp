#ifndef OCTOMYPLAN_HPP
#define OCTOMYPLAN_HPP

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

		void fail(QString msg)
		{
			qWarning()<<"E:" <<msg;
			m_ok=false;
			m_errors.append("ERROR:   "+msg);
		}

		void warn(QString msg)
		{
			qWarning()<<"W:" <<msg;
			m_errors.append("WARNING: "+msg);
		}

		void info (QString msg)
		{
			qDebug()<<"I:" <<msg;
			m_errors.append("INFO   : "+msg);
		}


		bool isOK()
		{
			return m_ok;
		}

		QString getErrorText()
		{
			QString out="";
			for(QVector<QString>::iterator i=m_errors.begin(), e=m_errors.end();e!=i;++i){
				out+=(*i)+"\n";
			}
			return out;
		}

};

#endif // OCTOMYPLAN_HPP
