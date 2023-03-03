#ifndef UNIONHIGHLIGHTER_HPP
#define UNIONHIGHLIGHTER_HPP


#include <QSyntaxHighlighter>
#include <QRegularExpression>

QT_BEGIN_NAMESPACE
class QTextDocument;
class QTextCharFormat;
QT_END_NAMESPACE

class UnionHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT
private:
	QVector<QSyntaxHighlighter *> highlighters;
	
public:
	explicit UnionHighlighter(QTextDocument *parent = nullptr);
	virtual ~UnionHighlighter();
	
protected:
	void highlightBlock(const QString &text) Q_DECL_OVERRIDE;
	
public:
	void registerHighlighter(QSyntaxHighlighter *hl);
	
};


#endif // UNIONHIGHLIGHTER_HPP
