#include "PlanHighlighter.hpp"


PlanHighlighter::PlanHighlighter(QTextDocument *parent)
	: QSyntaxHighlighter(parent)
{


	//Operator
	/*
	operatorFormat.setForeground(QColor("green"));
	operatorFormat.setFontItalic(true);
	QStringList operatorPatterns;
	operatorPatterns << "\\b+\\b" ; //<< "\\b\\-\\b" << "\\b\\*\\b" << "\\b\\!\\b"<< "\\b\\%\\b"<< "\\b\\&\\b" << "\\b\\/\\b";
	foreach (const QString &pattern, operatorPatterns) {
		rule.pattern = QRegExp(pattern);
		rule.format = operatorFormat;
		highlightingRules.append(rule);
	}
	*/

	//Paren
	parenFormat.setForeground(QColor("#ffffff"));
	QStringList parenPatterns;
	parenPatterns << "\\b\\{\\b" << "\\b\\}\\b"  << "\\b\\[\\b" << "\\b\\]\\b" << "\\b\\(\\b" << "\\b\\)\\b";
	foreach (const QString &pattern, parenPatterns) {
		highlightingRules.append(HighlightingRule (QRegExp(pattern), parenFormat));
	}

	//Keyword
	keywordFormat.setForeground(QColor("#e9d000"));
	keywordFormat.setFontWeight(QFont::Bold);
	QStringList keywordPatterns;
	keywordPatterns << "\\bplan\\b" << "\\bpuppet\\b" << "\\bmember\\b" << "\\blimb\\b";
	foreach (const QString &pattern, keywordPatterns) {
		highlightingRules.append(HighlightingRule (QRegExp(pattern), keywordFormat));
	}




	//Hub
	hubFormat.setForeground(QColor("#45de47"));
	hubFormat.setFontWeight(QFont::Bold);
	highlightingRules.append(HighlightingRule (QRegExp("\\bhub\\b"), hubFormat));

	//Remote
	remoteFormat.setForeground(QColor("#2faad0"));
	remoteFormat.setFontWeight(QFont::Bold);
	highlightingRules.append(HighlightingRule (QRegExp("\\bremote\\b"), remoteFormat));

	//Agent
	agentFormat.setForeground(QColor("#da3333"));
	agentFormat.setFontWeight(QFont::Bold);
	highlightingRules.append(HighlightingRule (QRegExp("\\bagent\\b"), agentFormat));

	operatorFormat.setFontWeight(QFont::Bold);
	operatorFormat.setForeground(QColor("#d0e900"));
	highlightingRules.append(HighlightingRule (QRegExp("\\b[\\\\+" + QRegExp::escape("e+-*/!=<>")+ "]+\\b"), operatorFormat));

	classFormat.setFontWeight(QFont::Bold);
	classFormat.setForeground(QColor("#d0e900"));
	highlightingRules.append(HighlightingRule (QRegExp("\\bQ[A-Za-z]+\\b"), classFormat));

	singleLineCommentFormat.setForeground(QColor("#666666"));
	highlightingRules.append(HighlightingRule (QRegExp("//[^\n]*"), singleLineCommentFormat));
	commentStartExpression = QRegExp("/\\*");
	commentEndExpression = QRegExp("\\*/");
	multiLineCommentFormat.setForeground(QColor("#666666"));

	stringFormat.setForeground(QColor("#00d0e9"));
	highlightingRules.append(HighlightingRule (QRegExp("\".*\""), stringFormat));

	functionFormat.setFontItalic(true);
	functionFormat.setForeground(QColor("#00e9d0"));
	highlightingRules.append(HighlightingRule (QRegExp("\\b[A-Za-z0-9_]+(?=\\()"), functionFormat));




}

void PlanHighlighter::highlightBlock(const QString &text){
	foreach (const HighlightingRule &rule, highlightingRules) {
		QRegExp expression(rule.pattern);
		int index = expression.indexIn(text);
		while (index >= 0) {
			int length = expression.matchedLength();
			setFormat(index, length, rule.format);
			index = expression.indexIn(text, index + length);
		}
	}
	setCurrentBlockState(0);

	int startIndex = 0;
	if (previousBlockState() != 1){
		startIndex = commentStartExpression.indexIn(text);
	}

	while (startIndex >= 0) {
		int endIndex = commentEndExpression.indexIn(text, startIndex);
		int commentLength;
		if (endIndex == -1) {
			setCurrentBlockState(1);
			commentLength = text.length() - startIndex;
		} else {
			commentLength = endIndex - startIndex + commentEndExpression.matchedLength();
		}
		setFormat(startIndex, commentLength, multiLineCommentFormat);
		startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
	}
}

