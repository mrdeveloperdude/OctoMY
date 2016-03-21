#include "PlanEditor.hpp"
#include "ui_PlanEditor.h"

#include "utility/Utility.hpp"
#include "plan/parser/PlanHighlighter.hpp"
//#include "../libparser/octomy_parser.hpp"


#include <QDebug>
#include <QPainter>

#include <QCompleter>
#include <QScrollBar>
#include <QAbstractItemModel>
#include <QStringListModel>
#include <QTextDocumentFragment>


CodeEditor::CodeEditor(QWidget *parent)
	: QPlainTextEdit(parent)
	, lineNumberArea(new LineNumberArea(this))
	, m_completer(0)
{


	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
	connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

	updateLineNumberAreaWidth(0);
	highlightCurrentLine();

	QCompleter *c=new QCompleter(this);
	c->setModel(modelFromFile(":/data/completion_wordlist.txt"));
	c->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	c->setCaseSensitivity(Qt::CaseInsensitive);
	c->setWrapAround(false);
	setCompleter(c);

}



QAbstractItemModel *CodeEditor::modelFromFile(const QString& fileName){
	QFile file(fileName);
	if (!file.open(QFile::ReadOnly)){
		return new QStringListModel(m_completer);
	}

#ifndef QT_NO_CURSOR
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
#endif
	QStringList words;

	while (!file.atEnd()) {
		QByteArray line = file.readLine();
		if (!line.isEmpty()){
			words << line.trimmed();
		}
	}

#ifndef QT_NO_CURSOR
	QApplication::restoreOverrideCursor();
#endif
	return new QStringListModel(words, m_completer);
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


void CodeEditor::blockIndent(bool in){
	qDebug()<<"BLOCK INDENT "<<(in?"IN":"OUT");
	QTextCursor cur = textCursor();
	int a = cur.anchor();
	int p = cur.position();

	cur.setPosition(a);
	cur.movePosition(QTextCursor::StartOfBlock,QTextCursor::MoveAnchor);
	a = cur.position();
	// save a new anchor at the beginning of the line of the selected text
	cur.setPosition(a);
	cur.setPosition(p, QTextCursor::KeepAnchor);
	// set a new selection with the new beginning
	QString str = (cur.selection()).toPlainText();
	QStringList list = str.split("\n");
	// get the selected text and split into lines

	const int l= list.count();
	if(in){
		for (int i = 0; i <l; i++){
			list[i].insert(0,"\t");
			p+=1;
		}
	}
	else{
		for (int i = 0; i < l; i++){
			QString s=list[i];
			if(s.startsWith("   ")){
				list[i]=s.mid(3);
				p-=3;
			}
			else if(s.startsWith("\t") ){
				list[i]=s.mid(1);
				p-=1;
			}
		}
	}
	//insert a space at the beginning of each line

	str=list.join("\n");
	cur.removeSelectedText();
	cur.insertText(str);
	// put the new text back
	cur.setPosition(a);
	cur.setPosition(p, QTextCursor::KeepAnchor);
	// reselect the text for more indents
	setTextCursor(cur);
	// put the whole thing back into the main text
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

		QColor lineColor = QColor("#44da3333");

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



void CodeEditor::setCompleter(QCompleter *c){
	if (0!=m_completer){
		QObject::disconnect(m_completer, 0, this, 0);
	}

	m_completer = c;

	if (0==m_completer){
		return;
	}

	m_completer->setWidget(this);
	m_completer->setCompletionMode(QCompleter::PopupCompletion);
	m_completer->setCaseSensitivity(Qt::CaseInsensitive);
	QObject::connect(m_completer, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));
}


QCompleter *CodeEditor::completer() const{
	return m_completer;
}

void CodeEditor::insertCompletion(const QString& completion){
	if (m_completer->widget() != this){
		return;
	}
	QTextCursor tc = textCursor();
	int extra = completion.length() - m_completer->completionPrefix().length();
	tc.movePosition(QTextCursor::Left);
	tc.movePosition(QTextCursor::EndOfWord);
	tc.insertText(completion.right(extra));
	setTextCursor(tc);
}

QString CodeEditor::textUnderCursor() const{
	QTextCursor tc = textCursor();
	tc.select(QTextCursor::WordUnderCursor);
	return tc.selectedText();
}


void CodeEditor::focusInEvent(QFocusEvent *e){
	if (0!=m_completer){
		m_completer->setWidget(this);
	}
	QPlainTextEdit::focusInEvent(e);
}


void CodeEditor::keyPressEvent(QKeyEvent *e){
	if (0!=m_completer && m_completer->popup()->isVisible()) {
		// The following keys are forwarded by the completer to the widget
		switch (e->key()) {
			case Qt::Key_Enter:
			case Qt::Key_Return:
			case Qt::Key_Escape:
			case Qt::Key_Tab:
			case Qt::Key_Backtab:
				e->ignore();
				return; // let the completer do default behavior
			default:
				break;
		}
	}


	if(e->key()== Qt::Key_Tab){
		e->accept();
		qDebug()<<"TAB";
		blockIndent(!(e->modifiers() & Qt::ShiftModifier));
		return;
	}
	else if(e->key() == Qt::Key_Backtab){
		e->accept();
		qDebug()<<"BACK TAB";
		blockIndent(false);
		return;
	}

	bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_Space); // CTRL+Space
	if (!m_completer || !isShortcut){ // do not process the shortcut when we have a completer
		QPlainTextEdit::keyPressEvent(e);
	}

	const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
	if (!m_completer || (ctrlOrShift && e->text().isEmpty())){
		return;
	}

	static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
	bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
	QString completionPrefix = textUnderCursor();

	if (!isShortcut && (hasModifier || e->text().isEmpty()|| completionPrefix.length() < 3 || eow.contains(e->text().right(1)))) {
		m_completer->popup()->hide();
		return;
	}

	if (completionPrefix != m_completer->completionPrefix()) {
		m_completer->setCompletionPrefix(completionPrefix);
		m_completer->popup()->setCurrentIndex(m_completer->completionModel()->index(0, 0));
	}
	QRect cr = cursorRect();
	cr.setWidth(m_completer->popup()->sizeHintForColumn(0) + m_completer->popup()->verticalScrollBar()->sizeHint().width());
	m_completer->complete(cr); // popup it up!
}



///////////////////////////////////////////////////////////////////////////////

PlanEditor::PlanEditor(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::PlanEditor)
	, highlighter(0)
{
	ui->setupUi(this);
	highlighter = new PlanHighlighter(ui->plainTextEditPlan->document());
}

PlanEditor::~PlanEditor(){
	qDebug()<<"SAVING PLAN TO "<<plan_fn;
	utility::stringToFile(plan_fn, ui->plainTextEditPlan->toPlainText());
	delete ui;
	delete highlighter;
}


void PlanEditor::configure(QString p){
	plan_fn=p;
	qDebug()<<"LOADING PLAN FROM "<<plan_fn;
	ui->plainTextEditPlan->setPlainText(utility::fileToString(plan_fn));
}



void PlanEditor::on_toolButtonTidy_clicked(){
	qDebug()<<"TIDY CLIIECKED";
}

void PlanEditor::on_toolButtonParse_clicked(){
	/*
	qDebug()<<"INSTANCIATING PARSER";
	OctomyParser p;
	qDebug()<<"DUMPING GRAMMAR FOR DEBUG PURPOSES";
	qDebug()<<p.toString();
	qDebug()<<"RUNNING PARSER";
	QString raw=ui->plainTextEditPlan->document()->toPlainText();
	qDebug()<<"PARSING RAW TEXT:"<<raw;
	bool ret=p.parse(raw);
	qDebug()<<"PARSING RESULTED IN "<<(ret?"SUCCESS":"FAILURE");
	*/
}
