#ifndef MARKDOWNHIGHLIGHTER_HPP
#define MARKDOWNHIGHLIGHTER_HPP

#include <QSyntaxHighlighter>
#include <QTextDocument>


#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QVector>
#include <QRegularExpression>



class QTextDocument;

class MarkdownHighlighter : public QSyntaxHighlighter
{
		Q_OBJECT
	private:
		struct HighlightingRule{
				QRegularExpression pattern;
				QTextCharFormat format;
				HighlightingRule(		QRegularExpression p,  QTextCharFormat f)
					: pattern(p)
					, format(f)
				{
				}
				HighlightingRule() {
				}
		};
	private:
		QVector<HighlightingRule> highlightingRules;

		QRegularExpression commentStartExpression;
		QRegularExpression commentEndExpression;

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

	public:
		MarkdownHighlighter(QTextDocument *parent = 0);


	protected:
		void highlightBlock(const QString &text) Q_DECL_OVERRIDE;



};

#endif // MARKDOWNHIGHLIGHTER_HPP
