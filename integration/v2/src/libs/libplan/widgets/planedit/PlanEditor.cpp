#include "PlanEditor.hpp"
#include "ui_PlanEditor.h"

#include "plan/PlanHighlighter.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "uptime/ConnectionType.hpp"
#include "utility/file/File.hpp"

#ifdef OC_USE_LIB_PARSER
#include "octomy_parser.hpp"
#endif

#include "OctomyParseContext.hpp"


PlanEditor::PlanEditor(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::PlanEditor)
	, highlighter(nullptr)
{
	OC_METHODGATE();
	ui->setupUi(this);
	if(!connect(ui->plainTextEditPlan, SIGNAL(textChanged()), this, SLOT(onTextChanged()), OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
	if(!connect(&saveTimer, SIGNAL(timeout()), this, SLOT(save()), OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}


	highlighter = OC_NEW PlanHighlighter(ui->plainTextEditPlan->document());
	ui->plainTextEditPlan->setFocus();
	saveTimer.setSingleShot(true);
	saveTimer.setTimerType(Qt::PreciseTimer);
	ui->plainTextEditParseErrors->setVisible(false);

}

PlanEditor::~PlanEditor()
{
	OC_METHODGATE();
	save();
	delete ui;
	ui=nullptr;
	delete highlighter;
	highlighter=nullptr;
}


void PlanEditor::configure(QString p)
{
	OC_METHODGATE();
	plan_fn=p;
	//qDebug()<<"LOADING PLAN FROM FILE: "<<plan_fn;
	QSignalBlocker sb(ui->plainTextEditPlan);
	ui->plainTextEditPlan->setPlainText(utility::file::fileToString(plan_fn));
}

void PlanEditor::setText(QString txt)
{
	OC_METHODGATE();
	ui->plainTextEditPlan->setPlainText(txt);
}

void PlanEditor::appendText(QString txt)
{
	OC_METHODGATE();
	ui->plainTextEditPlan->appendPlainText(txt);
}



void PlanEditor::tidy()
{
	OC_METHODGATE();
#ifdef OC_USE_LIB_PARSER
	qDebug()<<"TIDYING USING PARSER";
	OctomyParser p;
	QString raw=ui->plainTextEditPlan->document()->toPlainText();
	if(p.parse(raw)) {
		ui->plainTextEditPlan->setPlainText(p.treesToString());
	}
	QVector<ParseError> errors=p.getErrors();
	ui->plainTextEditPlan->setErrors(errors);
#else
	qDebug()<<"TIDYING DISABLED, NO PARSER AVAILABLE";
#endif
}

void PlanEditor::on_pushButtonTidy_clicked()
{
	tidy();
}

void PlanEditor::on_pushButtonParse_clicked()
{
#ifdef OC_USE_LIB_PARSER
	qDebug()<<"PARSE";
	OctomyParser p;
	QString raw=ui->plainTextEditPlan->document()->toPlainText();
	ui->plainTextEditParseErrors->setVisible(false);
	ui->plainTextEditParseErrors->clear();
	if(p.parse(raw)) {
		OctomyParseContext plan;
		bool ok=plan.fromParseTrees(p.getTrees());
		if(!ok) {
			ui->plainTextEditParseErrors->appendPlainText(plan.getErrorText());
			ui->plainTextEditParseErrors->setVisible(true);
			qWarning()<<"ERROR: Could not parse input into plan";
		}
	} else {
		qWarning()<<"ERROR: Could not parse raw plan into tree";
		ui->plainTextEditParseErrors->appendPlainText(p.getErrorText());
		ui->plainTextEditParseErrors->setVisible(true);
	}
	QVector<ParseError> errors=p.getErrors();
	ui->plainTextEditPlan->setErrors(errors);
	qWarning()<<"DONE PARSING";
#else
	qWarning()<<"NO PARSING DONE, PARSER NOT AVAILABLE";
#endif
}

void PlanEditor::onTextChanged()
{
	OC_METHODGATE();
	//Changes followed by X sec pause means we auto save
	saveTimer.start(10000);
}

void PlanEditor::save()
{
	OC_METHODGATE();
	ui->pushButtonSave->setEnabled(false);
	//qDebug()<<"SAVING: "<<plan_fn;
	QString txt=ui->plainTextEditPlan->toPlainText();
	//TODO: Should we really change data before saving like this? I mean will all users appreciate this?
	if(!txt.endsWith("\n")) {
		txt+="\n";
	}
	utility::file::stringToFile(plan_fn, txt);
	QTimer::singleShot(1000,this,SLOT(enableSaveButton()));
}

void PlanEditor::enableSaveButton()
{
	OC_METHODGATE();
	ui->pushButtonSave->setEnabled(true);
}


void PlanEditor::on_pushButtonSave_clicked()
{
	OC_METHODGATE();
	save();
}

void PlanEditor::keyPressEvent(QKeyEvent *e)
{
	OC_METHODGATE();
	Qt::Key key=static_cast<Qt::Key>(e->key());
	//qDebug()<<"KEY:"<<key;
	//Handle some stdandard keyboard shortcuts for text editor
	if(e->matches(QKeySequence::Save)) {
		e->accept();
		save();
		return;
	} else if(e->matches(QKeySequence::New)) {
		e->accept();
		ui->plainTextEditPlan->setPlainText("");
		return;
	} else if((Qt::Key_F==key) && (e->modifiers()==( Qt::ShiftModifier | Qt::ControlModifier))) {
		e->accept();
		tidy();
		return;
	}
}

void PlanEditor::on_pushButtonTest_clicked()
{
	OC_METHODGATE();
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

