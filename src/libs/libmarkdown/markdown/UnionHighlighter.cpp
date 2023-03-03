#include "UnionHighlighter.hpp"

#include <QString>
#include <QSyntaxHighlighter>

UnionHighlighter::UnionHighlighter(QTextDocument *parent)
	: QSyntaxHighlighter(parent)
{
	
}

UnionHighlighter::~UnionHighlighter(){
	for(auto hl:highlighters){
		if(hl){
			hl->deleteLater();
		}
	}
	highlighters.clear();
}


void UnionHighlighter::registerHighlighter(QSyntaxHighlighter *hl){
	highlighters<<hl;
}


void UnionHighlighter::highlightBlock(const QString &text)
{
	Q_UNUSED(text);
	for(auto hl:highlighters){
		if(hl){
			//TODO: Implement
			// https://stackoverflow.com/questions/75631684/methodx-is-a-protected-member-of-myclass-error-inside-class-that-extends-myc?noredirect
			//hl->highlightBlock(text);
		}
	}
}


