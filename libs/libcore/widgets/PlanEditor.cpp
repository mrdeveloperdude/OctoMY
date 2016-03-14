#include "PlanEditor.hpp"
#include "ui_PlanEditor.h"

#include "utility/Utility.hpp"
#include "plan/parser/PlanHighlighter.hpp"
#include "../libparser/octomy_parser.hpp"


#include <QDebug>

PlanEditor::PlanEditor(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::PlanEditor)
	, highlighter(0)
{
	ui->setupUi(this);
	qDebug()<<"LOADING PLAN FROM "<<plan_fn;
	highlighter = new PlanHighlighter(ui->plainTextEditPlan->document());
	ui->plainTextEditPlan->setPlainText(utility::fileToString(plan_fn));
}

PlanEditor::~PlanEditor(){
	qDebug()<<"SAVING PLAN TO "<<plan_fn;
	utility::stringToFile(plan_fn, ui->plainTextEditPlan->toPlainText());
	delete ui;
	delete highlighter;
}


void PlanEditor::configure(QString p){
	plan_fn=p;
}



void PlanEditor::on_toolButtonTidy_clicked(){
	qDebug()<<"TIDY CLIIECKED";
}

void PlanEditor::on_toolButtonParse_clicked(){
	qDebug()<<"INSTANCIATING PARSER";
	OctomyParser p;
	qDebug()<<"RUNNING PARSER";
	QString raw=ui->plainTextEditPlan->document()->toPlainText();
	qDebug()<<"PARSING RAW TEXT:"<<raw;
	bool ret=p.parse(raw);
	qDebug()<<"PARSING RESULTED IN "<<(ret?"SUCCESS":"FAILURE");
}
