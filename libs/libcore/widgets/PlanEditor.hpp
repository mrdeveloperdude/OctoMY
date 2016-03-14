#ifndef PLANEDITOR_HPP
#define PLANEDITOR_HPP

#include <QWidget>


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
