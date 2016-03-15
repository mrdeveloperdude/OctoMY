#include "PlanEditor.hpp"
#include "ui_PlanEditor.h"

#include "utility/Utility.hpp"
#include "plan/parser/PlanHighlighter.hpp"
#include "../libparser/octomy_parser.hpp"


#include <QDebug>
#include <QPainter>




CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent){
	lineNumberArea = new LineNumberArea(this);

	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
	connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

	updateLineNumberAreaWidth(0);
	highlightCurrentLine();
}



int CodeEditor::lineNumberAreaWidth(){
	int digits = 1;
	int max = qMax(1, blockCount());
	while (max >= 10) {
		max /= 10;
		++digits;
	}

	int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

	return space;
}



void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */){
	setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}



void CodeEditor::updateLineNumberArea(const QRect &rect, int dy){
	if (dy){
		lineNumberArea->scroll(0, dy);
	}
	else{
		lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
	}

	if (rect.contains(viewport()->rect())){
		updateLineNumberAreaWidth(0);
	}
}



void CodeEditor::resizeEvent(QResizeEvent *e){
	QPlainTextEdit::resizeEvent(e);
	QRect cr = contentsRect();
	lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}



void CodeEditor::highlightCurrentLine(){
	QList<QTextEdit::ExtraSelection> extraSelections;

	if (!isReadOnly()) {
		QTextEdit::ExtraSelection selection;

		QColor lineColor = QColor("#44FFee00");

		selection.format.setBackground(lineColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		selection.cursor = textCursor();
		selection.cursor.clearSelection();
		extraSelections.append(selection);
	}

	setExtraSelections(extraSelections);
}



void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event){
	QPainter painter(lineNumberArea);
	painter.fillRect(event->rect(), QColor("#66000000"));


	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();
	int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
	int bottom = top + (int) blockBoundingRect(block).height();

	while (block.isValid() && top <= event->rect().bottom()) {
		if (block.isVisible() && bottom >= event->rect().top()) {
			QString number = QString::number(blockNumber + 1);
			painter.setPen(QColor("#66ffffff"));
			painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(), Qt::AlignRight, number);
		}

		block = block.next();
		top = bottom;
		bottom = top + (int) blockBoundingRect(block).height();
		++blockNumber;
	}
}



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
