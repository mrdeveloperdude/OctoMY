#include "LineNumberArea.hpp"

#include "CodeEditor.hpp"

#include <QWidget>

namespace plan{
namespace editor{


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
}
