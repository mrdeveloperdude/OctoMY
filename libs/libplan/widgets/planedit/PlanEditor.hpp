#ifndef PLANEDITOR_HPP
#define PLANEDITOR_HPP




#include <QWidget>
#include <QTimer>
#include <QKeyEvent>



/*

#include <QWidget>
#include <QPlainTextEdit>


class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;

class LineNumberArea;

class QCompleter;
class QAbstractItemModel;

*/


class OctomyParse;

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
