#ifndef PLANEDITOR_HPP
#define PLANEDITOR_HPP

#include <QWidget>
#include <QPlainTextEdit>

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;

class LineNumberArea;


class CodeEditor : public QPlainTextEdit
{
	Q_OBJECT

public:
	CodeEditor(QWidget *parent = 0);

	void lineNumberAreaPaintEvent(QPaintEvent *event);
	int lineNumberAreaWidth();

protected:
	void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private slots:
	void updateLineNumberAreaWidth(int newBlockCount);
	void highlightCurrentLine();
	void updateLineNumberArea(const QRect &, int);

private:
	QWidget *lineNumberArea;
};


class LineNumberArea : public QWidget{
public:
	LineNumberArea(CodeEditor *editor) : QWidget(editor) {
		codeEditor = editor;
	}

	QSize sizeHint() const Q_DECL_OVERRIDE {
		return QSize(codeEditor->lineNumberAreaWidth(), 0);
	}

protected:
	void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE {
		codeEditor->lineNumberAreaPaintEvent(event);
	}

private:
	CodeEditor *codeEditor;
};

class PlanHighlighter;

namespace Ui {
	class PlanEditor;
}

class PlanEditor : public QWidget{
		Q_OBJECT

	private:
		Ui::PlanEditor *ui;
		PlanHighlighter *highlighter;
		QString plan_fn;
	public:
		explicit PlanEditor(QWidget *parent = 0);
		virtual ~PlanEditor();

		void configure(QString plan_fn);

	public slots:

		void on_toolButtonTidy_clicked();
		void on_toolButtonParse_clicked();

};

#endif // PLANEDITOR_HPP
