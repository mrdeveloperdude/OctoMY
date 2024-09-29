#ifndef LINENUMBERAREA_HPP
#define LINENUMBERAREA_HPP

#include <QWidget>

namespace markdown{

class CodeEditor;


class LineNumberArea : public QWidget{
	public:
		LineNumberArea(CodeEditor *editor);

		QSize sizeHint() const override ;

	protected:
		void paintEvent(QPaintEvent *event) override ;

	private:
		CodeEditor *codeEditor;
};

}

#endif // LINENUMBERAREA_HPP

