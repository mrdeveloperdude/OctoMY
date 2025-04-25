#include "plan/editor/CodeEditor.hpp"

#include "plan/editor/LineNumberArea.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QApplication>
#include <QCompleter>
#include <QDebug>
#include <QFile>
#include <QPainter>
#include <QScrollBar>
#include <QStringListModel>
#include <QTextBlock>
#include <QTextDocumentFragment>
#include <QTimer>
#include <QWidget>

namespace plan{
namespace editor{

CodeEditor::CodeEditor(QWidget *parent)
	: QPlainTextEdit(parent)
	, lineNumberArea(OC_NEW LineNumberArea(this))
	, m_completer(nullptr)
{
	OC_METHODGATE();
	if(!connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)), OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
	if(!connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)), OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
	if(!connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()), OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
	highlightCurrentLine();
	
	//setAcceptRichText(false);
	horizontalScrollBar()->hide();
	
	QCompleter *c=OC_NEW QCompleter(this);
	c->setModel(modelFromFile(":/data/completion_wordlist.txt"));
	c->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	c->setCaseSensitivity(Qt::CaseInsensitive);
	c->setWrapAround(false);
	setCompleter(c);
	
	updateLineNumberAreaWidth(0);
}



QAbstractItemModel *CodeEditor::modelFromFile(const QString& fileName)
{
	OC_METHODGATE();
	QFile file(fileName);
	if (!file.open(QFile::ReadOnly)) {
		return OC_NEW QStringListModel(m_completer);
	}
	
#ifndef QT_NO_CURSOR
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
#endif
	QStringList words;
	
	while (!file.atEnd()) {
		QByteArray line = file.readLine();
		if (!line.isEmpty()) {
			words << line.trimmed();
		}
	}
	
#ifndef QT_NO_CURSOR
	QApplication::restoreOverrideCursor();
#endif
	return OC_NEW QStringListModel(words, m_completer);
}

int CodeEditor::lineNumberAreaWidth()
{
	OC_METHODGATE();
	int digits = 1;
	int max = qMax(1, blockCount());
	while (max >= 10) {
		max /= 10;
		++digits;
	}
	int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
	return space;
}


void CodeEditor::blockIndent(bool in)
{
	OC_METHODGATE();
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
	if(in) {
		for (int i = 0; i <l; i++) {
			list[i].insert(0,"\t");
			p+=1;
		}
	} else {
		for (int i = 0; i < l; i++) {
			QString s=list[i];
			if(s.startsWith("   ")) {
				list[i]=s.mid(3);
				p-=3;
			} else if(s.startsWith("\t") ) {
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

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
	OC_METHODGATE();
	setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}



void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
	OC_METHODGATE();
	if (dy) {
		lineNumberArea->scroll(0, dy);
	} else {
		lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
	}
	
	if (rect.contains(viewport()->rect())) {
		updateLineNumberAreaWidth(0);
	}
}



void CodeEditor::resizeEvent(QResizeEvent *e)
{
	OC_METHODGATE();
	QPlainTextEdit::resizeEvent(e);
	QRect cr = contentsRect();
	lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}



void CodeEditor::highlightCurrentLine()
{
	OC_METHODGATE();
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




void CodeEditor::setErrors(QVector<ParseError> errors)
{
	OC_METHODGATE();
	this->errors=errors;
	
	//Errors
	QTextCharFormat errorFormat;
	errorFormat.setUnderlineColor( QColor( Qt::red ) );
	errorFormat.setUnderlineStyle( QTextCharFormat::WaveUnderline );
	errorFormat.setFontUnderline( true );
	
	for(QVector<ParseError>::iterator i=errors.begin(), e=errors.end(); i!=e; ++i) {
		ParseError error=*i;
		
		if(error.context.isValid()) {
			QTextCursor cursor;
			cursor.movePosition( QTextCursor::Start );
			cursor.movePosition( QTextCursor::Down, QTextCursor::MoveAnchor, error.context.line()- 1 );
			cursor.movePosition( QTextCursor::StartOfLine );
			cursor.movePosition( QTextCursor::Right, QTextCursor::MoveAnchor, 0 );
			cursor.movePosition( QTextCursor::WordRight, QTextCursor::KeepAnchor );
			setTextCursor( cursor ); // added
			QTextCharFormat oldfmt = currentCharFormat();
			errorFormat.setToolTip(error.message+"\n"+error.hint);
			setCurrentCharFormat( errorFormat );
			cursor.movePosition( QTextCursor::WordLeft );
			setTextCursor( cursor ); // added
			setCurrentCharFormat( oldfmt );
		}
	}
}



void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
	OC_METHODGATE();
	QPainter painter(lineNumberArea);
	painter.fillRect(event->rect(), QColor("#66000000"));
	
	
	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();
	int top = static_cast<int>(blockBoundingGeometry(block).translated(contentOffset()).top());
	int bottom = top + static_cast<int>(blockBoundingRect(block).height());
	
	while (block.isValid() && top <= event->rect().bottom()) {
		if (block.isVisible() && bottom >= event->rect().top()) {
			QString number = QString::number(blockNumber + 1);
			painter.setPen(QColor("#66ffffff"));
			painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(), Qt::AlignRight, number);
		}
		
		block = block.next();
		top = bottom;
		bottom = top + static_cast<int>(blockBoundingRect(block).height());
		++blockNumber;
	}
}



void CodeEditor::setCompleter(QCompleter *c)
{
	OC_METHODGATE();
	if (nullptr != m_completer) {
		QObject::disconnect(m_completer, nullptr, this, nullptr);
	}
	
	m_completer = c;
	
	if (nullptr == m_completer) {
		return;
	}
	
	m_completer->setWidget(this);
	m_completer->setCompletionMode(QCompleter::PopupCompletion);
	m_completer->setCaseSensitivity(Qt::CaseInsensitive);
	if(!connect(m_completer, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)))) {
		qWarning()<<"ERROR: could not connect";
	}
}


QCompleter *CodeEditor::completer() const
{
	OC_METHODGATE();
	return m_completer;
}

void CodeEditor::insertCompletion(const QString& completion)
{
	OC_METHODGATE();
	if (m_completer->widget() != this) {
		return;
	}
	QTextCursor tc = textCursor();
	int extra = completion.length() - m_completer->completionPrefix().length();
	tc.movePosition(QTextCursor::Left);
	tc.movePosition(QTextCursor::EndOfWord);
	tc.insertText(completion.right(extra));
	setTextCursor(tc);
}

QString CodeEditor::textUnderCursor() const
{
	OC_METHODGATE();
	QTextCursor tc = textCursor();
	tc.select(QTextCursor::WordUnderCursor);
	return tc.selectedText();
}


void CodeEditor::focusInEvent(QFocusEvent *e)
{
	OC_METHODGATE();
	if (nullptr != m_completer) {
		m_completer->setWidget(this);
	}
	QPlainTextEdit::focusInEvent(e);
}



void CodeEditor::keyPressEvent(QKeyEvent *e)
{
	OC_METHODGATE();
	const bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_Space); // CTRL+Space
	QTextCursor updateCursor = this->textCursor();
	const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
	static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
	const bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
	QString completionPrefix = textUnderCursor();
	
	if (nullptr != m_completer && m_completer->popup()->isVisible()) {
		qDebug()<<"completer visible, passing along key: "<<e->key();
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
	
	
	if(e->key()== Qt::Key_Tab) {
		e->accept();
		//qDebug()<<"TAB";
		blockIndent(!(e->modifiers() & Qt::ShiftModifier));
		return;
	} else if(e->key() == Qt::Key_Backtab) {
		e->accept();
		//qDebug()<<"BACK TAB";
		blockIndent(false);
		return;
	}
	
	
	
	
	
	if (nullptr != m_completer || !isShortcut) { // do not process the shortcut when we have a completer
		//qDebug()<<"no";
		QPlainTextEdit::keyPressEvent(e);
	}
	
	
	
	// End + Newline
	if (e->key() == Qt::Key_Return && e->modifiers() == Qt::ControlModifier) {
		updateCursor.movePosition(QTextCursor::EndOfBlock,QTextCursor::MoveAnchor);
		updateCursor.insertText("\n");
		this->setTextCursor(updateCursor);
	}
	// Auto-indentation
	if (e->key() == Qt::Key_Return && (e->modifiers() == Qt::NoModifier || e->modifiers() == Qt::ControlModifier || e->modifiers() == Qt::ShiftModifier)) {
		QString data = toPlainText();
		int cursorPosition = updateCursor.position();
		int i;
		for (i=cursorPosition-2; i>=0; i--) {
			if (data.mid(i,1) == "\n") {
				break;
			}
		}
		while (data.mid(i+1,1) == "\t") {
			updateCursor.insertText("\t");
			i++;
		}
	}
	
	
	if (!m_completer || (ctrlOrShift && e->text().isEmpty())) {
		//qDebug()<<"c|s";
		return;
	}
	
	
	
	if (!isShortcut && (hasModifier || e->text().isEmpty()|| completionPrefix.length() < 3 || eow.contains(e->text().right(1)))) {
		//qDebug()<<"close @ eow";
		m_completer->popup()->hide();
		return;
	}
	
	if (completionPrefix != m_completer->completionPrefix()) {
		//qDebug()<<"popup @ eow";
		m_completer->setCompletionPrefix(completionPrefix);
		m_completer->popup()->setCurrentIndex(m_completer->completionModel()->index(0, 0));
	}
	//qDebug()<<"popup @ eow 2";
	QRect cr = cursorRect();
	cr.setWidth(m_completer->popup()->sizeHintForColumn(0) + m_completer->popup()->verticalScrollBar()->sizeHint().width());
	m_completer->complete(cr); // popup it up!
}

}
}

