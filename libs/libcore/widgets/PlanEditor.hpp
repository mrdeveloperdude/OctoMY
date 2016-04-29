#ifndef PLANEDITOR_HPP
#define PLANEDITOR_HPP



#include "../../libs/libparser/octomy_parser.hpp"



#include <QWidget>
#include <QPlainTextEdit>
#include <QTimer>

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;

class LineNumberArea;

class QCompleter;
class QAbstractItemModel;

class CodeEditor : public QPlainTextEdit
{
		Q_OBJECT
	private:
		QWidget *lineNumberArea;
		QCompleter *m_completer;
		QVector<ParseError> errors;

	public:
		CodeEditor(QWidget *parent = 0);

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
		void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
		void focusInEvent(QFocusEvent *e) Q_DECL_OVERRIDE;
		void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

	private slots:
		void insertCompletion(const QString &completion);
		void updateLineNumberAreaWidth(int newBlockCount);
		void highlightCurrentLine();
		void updateLineNumberArea(const QRect &, int);
};


///////////////////////////////////////////////////////////////////////////////



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

///////////////////////////////////////////////////////////////////////////////



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
		QTimer saveTimer;

	public:
		explicit PlanEditor(QWidget *parent = 0);
		virtual ~PlanEditor();

		void configure(QString plan_fn);

		void setText(QString);
		void appendText(QString);

	protected:
		void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
	public slots:
		void onTextChanged();
		void save();
		void tidy();
		void enableSaveButton();
		void on_pushButtonTidy_clicked();
		void on_pushButtonParse_clicked();

	private slots:
		void on_pushButtonSave_clicked();
		void on_pushButtonTest_clicked();
};

#endif // PLANEDITOR_HPP
