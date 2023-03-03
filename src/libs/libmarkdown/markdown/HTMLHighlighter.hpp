#ifndef HTMLHIGHLIGHTER_HPP
#define HTMLHIGHLIGHTER_HPP

#include <QSyntaxHighlighter>
#include <QRegularExpression>

QT_BEGIN_NAMESPACE
class QTextDocument;
class QTextCharFormat;
QT_END_NAMESPACE

class HTMLHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT
	
public:
	explicit HTMLHighlighter(QTextDocument *parent = 0);
	
protected:
	void highlightBlock(const QString &text) Q_DECL_OVERRIDE;
	
private:
	
	enum States {
		None,
		Tag,
		Comment,
		Quote
	};
	
	struct HighlightingRule
	{
		QRegularExpression pattern;
		QTextCharFormat format;
	};
	QVector<HighlightingRule> startTagRules;
	QVector<HighlightingRule> endTagRules;
	
	QRegularExpression openTag;
	QRegularExpression closeTag;
	QTextCharFormat edgeTagFormat;
	QTextCharFormat insideTagFormat;
	
	QRegularExpression commentStartExpression;
	QRegularExpression commentEndExpression;
	QTextCharFormat multiLineCommentFormat;
	
	QRegularExpression quotes;
	QTextCharFormat quotationFormat;
	QTextCharFormat tagsFormat;
};

#endif // HTMLHIGHLIGHTER_HPP
