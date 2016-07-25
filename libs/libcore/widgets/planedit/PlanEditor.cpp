#include "PlanEditor.hpp"
#include "ui_PlanEditor.h"


#include "plan/parser/PlanHighlighter.hpp"

#include "../libparser/octomy_parser.hpp"
#include "../libparser/OctomyParseContext.hpp"



/*
#include "utility/Utility.hpp"

#include "../libparser/octomy_parser.hpp"
#include "../libparser/OctomyParseContext.hpp"

#include "CodeEditor.hpp"

#include <QDebug>
#include <QPainter>
#include <QTimer>

#include <QCompleter>
#include <QScrollBar>
#include <QAbstractItemModel>
#include <QStringListModel>
#include <QTextDocumentFragment>

*/

PlanEditor::PlanEditor(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::PlanEditor)
	, highlighter(0)
{
	ui->setupUi(this);
	if(!connect(ui->plainTextEditPlan, SIGNAL(textChanged()), this, SLOT(onTextChanged()))){
		qWarning()<<"ERROR: Could not connect";
	}
	if(!connect(&saveTimer, SIGNAL(timeout()), this, SLOT(save()))){
		qWarning()<<"ERROR: Could not connect";
	}


	highlighter = new PlanHighlighter(ui->plainTextEditPlan->document());
	ui->plainTextEditPlan->setFocus();
	saveTimer.setSingleShot(true);
	saveTimer.setTimerType(Qt::PreciseTimer);
	ui->plainTextEditParseErrors->setVisible(false);

}

PlanEditor::~PlanEditor(){
	save();
	delete ui;
	delete highlighter;
}


void PlanEditor::configure(QString p){
	plan_fn=p;
	//qDebug()<<"LOADING PLAN FROM FILE: "<<plan_fn;
	QSignalBlocker sb(ui->plainTextEditPlan);
	ui->plainTextEditPlan->setPlainText(utility::fileToString(plan_fn));
}

void PlanEditor::setText(QString txt){
	ui->plainTextEditPlan->setPlainText(txt);
}

void PlanEditor::appendText(QString txt){
	ui->plainTextEditPlan->appendPlainText(txt);
}



void PlanEditor::tidy(){
	qDebug()<<"TIDY";
	OctomyParser p;
	QString raw=ui->plainTextEditPlan->document()->toPlainText();
	if(p.parse(raw)){
		ui->plainTextEditPlan->setPlainText(p.treesToString());
	}
	QVector<ParseError> errors=p.getErrors();
	ui->plainTextEditPlan->setErrors(errors);
}

void PlanEditor::on_pushButtonTidy_clicked()
{
	tidy();
}

void PlanEditor::on_pushButtonParse_clicked(){
	qDebug()<<"PARSE";
	OctomyParser p;
	QString raw=ui->plainTextEditPlan->document()->toPlainText();
	ui->plainTextEditParseErrors->setVisible(false);
	ui->plainTextEditParseErrors->clear();
	if(p.parse(raw)){
		OctomyParseContext plan;
		bool ok=plan.fromParseTrees(p.getTrees());
		if(!ok){
			ui->plainTextEditParseErrors->appendPlainText(plan.getErrorText());
			ui->plainTextEditParseErrors->setVisible(true);
			qWarning()<<"ERROR: Could not parse input into plan";
		}
	}
	else{
		qWarning()<<"ERROR: Could not parse raw plan into tree";
		ui->plainTextEditParseErrors->appendPlainText(p.getErrorText());
		ui->plainTextEditParseErrors->setVisible(true);
	}
	QVector<ParseError> errors=p.getErrors();
	ui->plainTextEditPlan->setErrors(errors);
	qWarning()<<"DONE PARSING";
}

void PlanEditor::onTextChanged()
{
	//Changes followed by X sec pause means we auto save
	saveTimer.start(10000);
}

void PlanEditor::save()
{
	ui->pushButtonSave->setEnabled(false);
	qDebug()<<"SAVING: "<<plan_fn;
	QString txt=ui->plainTextEditPlan->toPlainText();
	//TODO: Should we really change data before saving like this? I mean will all users appreciate this?
	if(!txt.endsWith("\n")){
		txt+="\n";
	}
	utility::stringToFile(plan_fn, txt);
	QTimer::singleShot(1000,this,SLOT(enableSaveButton()));
}

void PlanEditor::enableSaveButton()
{
	ui->pushButtonSave->setEnabled(true);
}


void PlanEditor::on_pushButtonSave_clicked()
{
	save();
}

void PlanEditor::keyPressEvent(QKeyEvent *e)
{
	Qt::Key key=static_cast<Qt::Key>(e->key());
	//qDebug()<<"KEY:"<<key;
	//Handle some stdandard keyboard shortcuts for text editor
	if(e->matches(QKeySequence::Save)){
		e->accept();
		save();
		return;
	}
	else if(e->matches(QKeySequence::New)){
		e->accept();
		ui->plainTextEditPlan->setPlainText("");
		return;
	}
	else if((Qt::Key_F==key) && (e->modifiers()==( Qt::ShiftModifier | Qt::ControlModifier))){
		e->accept();
		tidy();
		return;
	}
}

void PlanEditor::on_pushButtonTest_clicked()
{
	qDebug()<<"TEST";
	QString txt="plan{\n"
				" agent{\n"
				"  puppet{\n"
				"   member{\n"
				"   }\n"
				"  }\n"
				" }\n"
				" agent{\n"
				"  puppet{\n"
				"   member{\n"
				"    lol1 = 123;\n"
				"    lol2=321 \n"
				"    lol3 = 3.14159265;\n"
				"    lol4=7.43e-23 \n"
				"    lol5= true;\n"
				"    lol6 =false   \n"
				"    lol7  = \"foo\";\n"
				"    lol8  =  \"bar\"\n"
				"   }\n"
				"  }\n"
				" }\n"
				" hub{\n"
				" }\n"
				" remote{\n"
				" }\n"
				"}\n";
	ui->plainTextEditPlan->setPlainText(txt);
}

