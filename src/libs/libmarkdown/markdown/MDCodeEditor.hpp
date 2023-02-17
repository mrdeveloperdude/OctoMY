#ifndef CODEEDITOR_HPP
#define CODEEDITOR_HPP


#include <QPlainTextEdit>
#include <QVector>

class QWidget;
class QCompleter;
class QAbstractItemModel;

namespace markdown{

class CodeEditor : public QPlainTextEdit
{
	Q_OBJECT
private:
	QWidget *lineNumberArea;
	QCompleter *m_completer;

public:
	CodeEditor(QWidget *parent = nullptr);
	void lineNumberAreaPaintEvent(QPaintEvent *event);
	int lineNumberAreaWidth();

private:
	QString textUnderCursor() const;
	QAbstractItemModel *modelFromFile(const QString& fileName);
	void blockIndent(bool in=true);

public:
	void setCompleter(QCompleter *completer);
	QCompleter *completer() const;

protected:
	void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
	void focusInEvent(QFocusEvent *e) Q_DECL_OVERRIDE;
	void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private slots:
	void insertCompletion(const QString &completion);
	void updateLineNumberAreaWidth(int newBlockCount);
	void highlightCurrentLine();
	void updateLineNumberArea(const QRect &, int);
};

}
#endif
// CODEEDITOR_HPP
