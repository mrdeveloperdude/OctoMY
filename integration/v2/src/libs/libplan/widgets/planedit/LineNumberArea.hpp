#ifndef LINENUMBERAREA_HPP
#define LINENUMBERAREA_HPP

#include <QWidget>

class CodeEditor;


class LineNumberArea : public QWidget{
	public:
		LineNumberArea(CodeEditor *editor);

		QSize sizeHint() const Q_DECL_OVERRIDE ;

	protected:
		void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE ;

	private:
		CodeEditor *codeEditor;
};

#endif // LINENUMBERAREA_HPP
