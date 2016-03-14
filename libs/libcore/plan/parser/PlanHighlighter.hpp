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
	struct HighlightingRule
	{
		QRegExp pattern;
		QTextCharFormat format;
	};
	QVector<HighlightingRule> highlightingRules;

	QRegExp commentStartExpression;
	QRegExp commentEndExpression;

	QTextCharFormat keywordFormat;
	QTextCharFormat classFormat;
	QTextCharFormat singleLineCommentFormat;
	QTextCharFormat multiLineCommentFormat;
	QTextCharFormat quotationFormat;
	QTextCharFormat functionFormat;
};

#endif // PLANHIGHLIGHTER_HPP
