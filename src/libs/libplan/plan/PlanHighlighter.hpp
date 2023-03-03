#ifndef PLANHIGHLIGHTER_HPP
#define PLANHIGHLIGHTER_HPP

#include <QSyntaxHighlighter>
#include <QTextDocument>


#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QVector>
#include <QRegularExpression>



class QTextDocument;

class PlanHighlighter : public QSyntaxHighlighter
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
	QTextCharFormat multiLineCommentFormat;
	
public:
	PlanHighlighter(QTextDocument *parent = 0);
	
	
protected:
	void highlightBlock(const QString &text) Q_DECL_OVERRIDE;
	
	
	
};

#endif // PLANHIGHLIGHTER_HPP
