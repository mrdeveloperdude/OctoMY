#include "QHexEdit.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

QHexEdit::QHexEdit(QWidget *parent): QFrame(parent)
{
	OC_METHODGATE();
	this->_vscrollbar = OC_NEW QScrollBar(Qt::Vertical);
	this->_scrollarea = OC_NEW QScrollArea();
	this->_hexedit_p = OC_NEW QHexEditPrivate(this->_scrollarea, this->_vscrollbar);

	/* Forward QHexEditPrivate's Signals */
	connect(this->_hexedit_p, SIGNAL(visibleLinesChanged()), this, SIGNAL(visibleLinesChanged()));
	connect(this->_hexedit_p, SIGNAL(positionChanged(qint64)), this, SIGNAL(positionChanged(qint64)));
	connect(this->_hexedit_p, SIGNAL(selectionChanged(qint64)), this, SIGNAL(selectionChanged(qint64)));
	connect(this->_hexedit_p, SIGNAL(verticalScrollBarValueChanged(int)), this, SIGNAL(verticalScrollBarValueChanged(int)));

	this->_scrollarea->setFocusPolicy(Qt::NoFocus);
	this->_scrollarea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); /* Do not show vertical QScrollBar!!! */
	this->_scrollarea->setFrameStyle(QFrame::NoFrame);
	this->_scrollarea->setWidgetResizable(true);
	this->_scrollarea->setWidget(this->_hexedit_p);

	this->setFocusPolicy(Qt::NoFocus);

	this->_hlayout = OC_NEW QHBoxLayout();
	this->_hlayout->setSpacing(0);
	this->_hlayout->setContentsMargins(0,0,0,0);
	this->_hlayout->addWidget(this->_scrollarea);
	this->_hlayout->addWidget(this->_vscrollbar);

	this->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	this->setLayout(this->_hlayout);
}

void QHexEdit::undo()
{
	OC_METHODGATE();
	this->_hexedit_p->undo();
}

void QHexEdit::setData(QHexEditData *hexeditdata)
{
	OC_METHODGATE();
	this->_hexedit_p->setData(hexeditdata);
}

void QHexEdit::selectPos(qint64 pos)
{
	OC_METHODGATE();
	this->setSelectionLength(pos, 1);
}

void QHexEdit::setSelection(qint64 start, qint64 end)
{
	OC_METHODGATE();
	this->_hexedit_p->setSelection(start, end);
}

void QHexEdit::setSelectionLength(qint64 start, qint64 length)
{
	OC_METHODGATE();
	this->setSelection(start, (start + length) - 1);
}

void QHexEdit::highlightBackground(qint64 start, qint64 end, const QColor &color)
{
	OC_METHODGATE();
	this->_hexedit_p->highlightBackground(start, end, color);
}

void QHexEdit::highlightForeground(qint64 start, qint64 end, const QColor &color)
{
	OC_METHODGATE();
	this->_hexedit_p->highlightForeground(start, end, color);
}

void QHexEdit::clearHighlight(qint64 start, qint64 end)
{
	OC_METHODGATE();
	this->_hexedit_p->clearHighlight(start, end);
}

void QHexEdit::clearHighlight()
{
	OC_METHODGATE();
	this->_hexedit_p->clearHighlight();
}

void QHexEdit::commentRange(qint64 start, qint64 end, const QString &comment)
{
	OC_METHODGATE();
	this->_hexedit_p->commentRange(start, end, comment);
}

void QHexEdit::uncommentRange(qint64 start, qint64 end)
{
	OC_METHODGATE();
	this->_hexedit_p->uncommentRange(start, end);
}

void QHexEdit::clearComments()
{
	OC_METHODGATE();
	this->_hexedit_p->clearComments();
}

void QHexEdit::setVerticalScrollBarValue(int value)
{
	OC_METHODGATE();
	this->_hexedit_p->setVerticalScrollBarValue(value);
}

void QHexEdit::scroll(QWheelEvent *event)
{
	OC_METHODGATE();
	this->_hexedit_p->scroll(event);
}

void QHexEdit::setCursorPos(qint64 pos)
{
	OC_METHODGATE();
	this->_hexedit_p->setCursorPos(pos);
}

void QHexEdit::paste()
{
	OC_METHODGATE();
	this->_hexedit_p->paste();
}

void QHexEdit::pasteHex()
{
	OC_METHODGATE();
	this->_hexedit_p->pasteHex();
}

void QHexEdit::selectAll()
{
	OC_METHODGATE();
	this->_hexedit_p->setSelection(0, -1);
}

int QHexEdit::addressWidth()
{
	OC_METHODGATE();
	return this->_hexedit_p->addressWidth();
}

int QHexEdit::visibleLinesCount()
{
	OC_METHODGATE();
	return this->_hexedit_p->visibleLinesCount();
}

QHexEditData *QHexEdit::data()
{
	OC_METHODGATE();
	return this->_hexedit_p->data();
}

qint64 QHexEdit::indexOf(QByteArray &ba, qint64 start)
{
	OC_METHODGATE();
	return this->_hexedit_p->indexOf(ba, start);
}

qint64 QHexEdit::baseAddress()
{
	OC_METHODGATE();
	return this->_hexedit_p->baseAddress();
}

qint64 QHexEdit::cursorPos()
{
	OC_METHODGATE();
	return this->_hexedit_p->cursorPos();
}

qint64 QHexEdit::selectionStart()
{
	OC_METHODGATE();
	return this->_hexedit_p->selectionStart();
}

qint64 QHexEdit::selectionEnd()
{
	OC_METHODGATE();
	return this->_hexedit_p->selectionEnd();
}

qint64 QHexEdit::selectionLength()
{
	OC_METHODGATE();
	return this->_hexedit_p->selectionEnd() - this->_hexedit_p->selectionStart();
}

void QHexEdit::setFont(const QFont &f)
{
	OC_METHODGATE();
	this->_hexedit_p->setFont(f);
}

void QHexEdit::setBaseAddress(qint64 ba)
{
	OC_METHODGATE();
	return this->_hexedit_p->setBaseAddress(ba);
}

qint64 QHexEdit::visibleStartOffset()
{
	OC_METHODGATE();
	return this->_hexedit_p->visibleStartOffset();
}

qint64 QHexEdit::visibleEndOffset()
{
	OC_METHODGATE();
	return this->_hexedit_p->visibleEndOffset();
}

bool QHexEdit::readOnly()
{
	OC_METHODGATE();
	return this->_hexedit_p->readOnly();
}

void QHexEdit::setReadOnly(bool b)
{
	OC_METHODGATE();
	this->_hexedit_p->setReadOnly(b);
}

void QHexEdit::copy()
{
	OC_METHODGATE();
	this->_hexedit_p->copy();
}

void QHexEdit::copyHex()
{
	OC_METHODGATE();
	this->_hexedit_p->copyHex();
}

void QHexEdit::cut()
{
	OC_METHODGATE();
	this->_hexedit_p->cut();
}

void QHexEdit::redo()
{
	OC_METHODGATE();
	this->_hexedit_p->redo();
}
