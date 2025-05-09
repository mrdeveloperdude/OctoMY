#ifndef CODEEDITOR_HPP
#define CODEEDITOR_HPP

#include "parser/ParseError.hpp"

#include <QPlainTextEdit>
#include <QVector>

class QWidget;
class QCompleter;
class QAbstractItemModel;

namespace plan{
namespace editor{

class CodeEditor : public QPlainTextEdit
{
	Q_OBJECT
private:
	QWidget *lineNumberArea;
	QCompleter *m_completer;
	QVector<ParseError> errors;

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
	void setErrors(QVector<ParseError> errors);

protected:
	void keyPressEvent(QKeyEvent *e) override;
	void focusInEvent(QFocusEvent *e) override;
	void resizeEvent(QResizeEvent *event) override;

private slots:
	void insertCompletion(const QString &completion);
	void updateLineNumberAreaWidth(int newBlockCount);
	void highlightCurrentLine();
	void updateLineNumberArea(const QRect &, int);
};

}
}

#endif
// CODEEDITOR_HPP
