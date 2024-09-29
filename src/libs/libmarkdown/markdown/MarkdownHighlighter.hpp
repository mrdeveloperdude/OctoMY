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
public:
	MarkdownHighlighter(QTextDocument *parent = 0);
	
protected:
	void highlightBlock(const QString &text) override;
	
};

#endif // MARKDOWNHIGHLIGHTER_HPP
