#include "PlanEditor.hpp"
#include "ui_PlanEditor.h"

#include "node/Node.hpp"
#include "plan/Plan.hpp"
#include "plan/book/PlanBook.hpp"
#include "plan/editor/PlanHighlighter.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "utility/string/String.hpp"
#include "utility/time/HumanTime.hpp"

#ifdef OC_USE_LIB_PARSER
#include "octomy_parser.hpp"
#endif


namespace plan{
namespace editor{

PlanEditor::PlanEditor(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::PlanEditor)
	, mHighlighter(nullptr)
{
	OC_METHODGATE();
	ui->setupUi(this);
	if(!connect(ui->plainTextEditPlan, &plan::editor::CodeEditor::textChanged, this, &PlanEditor::onTextChanged, OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
	if(!connect(&mSaveTimer, &QTimer::timeout, this, &PlanEditor::synchronizePlan, OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
	
	if(!connect(&mClockTimer, &QTimer::timeout, this, &PlanEditor::clockUpdate, OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
	mHighlighter = OC_NEW PlanHighlighter(ui->plainTextEditPlan->document());
	ui->plainTextEditPlan->setFocus();
	mSaveTimer.setTimerType(Qt::VeryCoarseTimer);
	mSaveTimer.setSingleShot(true);
	mClockTimer.setTimerType(Qt::PreciseTimer);
	mClockTimer.start(1000);
	ui->plainTextEditParseErrors->setVisible(false);
	
}

PlanEditor::~PlanEditor()
{
	OC_METHODGATE();
	synchronizePlan();
	delete ui;
	ui=nullptr;
	delete mHighlighter;
	mHighlighter=nullptr;
}


void PlanEditor::configure(QSharedPointer<Node> n)
{
	OC_METHODGATE();
	mNode = n;
	//qDebug()<<"LOADING PLAN FROM FILE: "<<plan_fn;
	QSignalBlocker sb(ui->plainTextEditPlan);
	if(mNode){
		auto planBook = mNode->planBook();
		if(planBook){
			auto id = planBook->defaultPlan();
			if(!id.isEmpty()){
				loadPlan(id);
			}
			else{
				qWarning() << "No default plan";
			}
		}
		else{
			qWarning() << "No plan book";
		}
	}
	else{
		qWarning() << "No node";
	}
}


void PlanEditor::setPlan(QSharedPointer<Plan> plan){
	OC_METHODGATE();
	if(plan){
		ui->lineEditPlanName->setText(plan->name());
		ui->plainTextEditPlan->setPlainText(plan->content());
		mPlan = plan;
	}
}


void PlanEditor::loadPlan(const QString &id)
{
	OC_METHODGATE();
	if(mNode){
		auto planBook = mNode->planBook();
		if(planBook){
			auto plan = planBook->planByID(id);
			if(plan){
				setPlan(plan);
			}
			else{
				qWarning() << "No plan";
			}
		}
		else{
			qWarning() << "No plan book";
		}
	}
	else{
		qWarning() << "No node";
	}
}


void PlanEditor::tidyPlan()
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

void PlanEditor::parsePlan()
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
	mSaveTimer.start(10000);
}


void PlanEditor::enableSaveButton()
{
	OC_METHODGATE();
	ui->pushButtonSave->setEnabled(true);
}

void PlanEditor::synchronizePlan()
{
	OC_METHODGATE();
	ui->pushButtonSave->setEnabled(false);
	if(!mPlan.isNull()){
		//qDebug()<<"SAVING: "<<plan_fn;
		QString txt=ui->plainTextEditPlan->toPlainText();
		//TODO: Should we really change data before saving like this? I mean will all users appreciate this?
		if(!txt.endsWith("\n")) {
			txt+="\n";
		}
		mPlan->setContent(txt);
		mPlan->setName(ui->lineEditPlanName->text());
		if(mNode){
			auto planBook = mNode->planBook();
			if(planBook){
				planBook->upsertPlan(mPlan);
				planBook->synchronize();
			}
			else{
				qWarning() << "No plan book";
			}
		}
		else{
			qWarning() << "No node";
		}
	}
	QTimer::singleShot(1000, this, &PlanEditor::enableSaveButton);
}


void PlanEditor::clockUpdate(){
	OC_METHODGATE();
	QString sinceCreatedText;
	QString sinceUpdatedText;
	if(!mPlan.isNull()){
		auto now = utility::time::currentMsecsSinceEpoch<quint64>();
		quint64 sinceCreatedMS{now-mPlan->createdMS()};
		quint64 sinceUpdatedMS{now-mPlan->lastUpdatedMS()};
		sinceCreatedText = utility::string::humanReadableElapsedMS(sinceCreatedMS);
		sinceUpdatedText = utility::string::humanReadableElapsedMS(sinceUpdatedMS);
	}
	else{
		sinceCreatedText = "N/A";
		sinceUpdatedText = "N/A";
	}
	ui->labelCreatedTimestamp->setText(sinceCreatedText);
	ui->labelUpdatedTimestamp->setText(sinceUpdatedText);
}


void PlanEditor::keyPressEvent(QKeyEvent *e)
{
	OC_METHODGATE();
	Qt::Key key=static_cast<Qt::Key>(e->key());
	//qDebug()<<"KEY:"<<key;
	//Handle some stdandard keyboard shortcuts for text editor
	if(e->matches(QKeySequence::Save)) {
		e->accept();
		synchronizePlan();
		return;
	} else if(e->matches(QKeySequence::New)) {
		e->accept();
		// TODO: Handle creation of new plan from editor
		ui->plainTextEditPlan->setPlainText("");
		return;
	} else if((Qt::Key_F==key) && (e->modifiers()==( Qt::ShiftModifier | Qt::ControlModifier))) {
		e->accept();
		tidyPlan();
		return;
	}
}

void PlanEditor::loadDefaultPlan()
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



void PlanEditor::setPlanName(const QString &name){
	OC_METHODGATE();
	if(mPlan){
		mPlan->setName(name);
	}
}


}
}
