#ifndef PLANHIGHLIGHTER_HPP
#define PLANHIGHLIGHTER_HPP

#include <QSyntaxHighlighter>
#include <QTextDocument>


#include <QSyntaxHighlighter>
#include <QTextCharFormat>

class QTextDocument;

class PlanHighlighter : public QSyntaxHighlighter
{
		Q_OBJECT

	public:
		PlanHighlighter(QTextDocument *parent = 0);

	protected:
		void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

	private:
		struct HighlightingRule{
				QRegExp pattern;
				QTextCharFormat format;
				HighlightingRule(		QRegExp p,  QTextCharFormat f)
					: pattern(p)
					, format(f)
				{

				}
				HighlightingRule() {

				}
		};
		QVector<HighlightingRule> highlightingRules;

		QRegExp commentStartExpression;
		QRegExp commentEndExpression;

		QTextCharFormat operatorFormat;
		QTextCharFormat parenFormat;
		QTextCharFormat keywordFormat;
		QTextCharFormat hubFormat;
		QTextCharFormat remoteFormat;
		QTextCharFormat agentFormat;
		QTextCharFormat classFormat;
		QTextCharFormat singleLineCommentFormat;
		QTextCharFormat multiLineCommentFormat;
		QTextCharFormat stringFormat;
		QTextCharFormat functionFormat;
};

#endif // PLANHIGHLIGHTER_HPP
