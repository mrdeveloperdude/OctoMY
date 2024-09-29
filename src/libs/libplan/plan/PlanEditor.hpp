#ifndef PLANEDITOR_HPP
#define PLANEDITOR_HPP


#include <QWidget>
#include <QTimer>
#include <QKeyEvent>


class OctomyParse;

class PlanHighlighter;

namespace Ui
{
class PlanEditor;
}


namespace plan{

class PlanEditor : public QWidget
{
	Q_OBJECT
	
private:
	Ui::PlanEditor *ui;
	PlanHighlighter *highlighter;
	QString plan_fn;
	QTimer saveTimer;
	
public:
	explicit PlanEditor(QWidget *parent = nullptr);
	virtual ~PlanEditor() override;
	
public:
	void configure(QString plan_fn);
	void setText(QString);
	void appendText(QString);
	
protected:
	void keyPressEvent(QKeyEvent *e) override;
	
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

}

#endif
// PLANEDITOR_HPP

