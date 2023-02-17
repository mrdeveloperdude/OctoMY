#include "LineNumberArea.hpp"

#include "markdown/MDCodeEditor.hpp"

namespace markdown{
Q_NAMESPACE

LineNumberArea::LineNumberArea(CodeEditor *editor)
	: QWidget(editor)
	, codeEditor(editor)
{
	
}

QSize LineNumberArea::sizeHint() const {
	return QSize(codeEditor->lineNumberAreaWidth(), 0);
}


void LineNumberArea::paintEvent(QPaintEvent *event) {
	codeEditor->lineNumberAreaPaintEvent(event);
}


}
