#include "MarkdownHighlighter.hpp"


MarkdownHighlighter::MarkdownHighlighter(QTextDocument *parent)
	: QSyntaxHighlighter(parent)
{


	//Operator
	/*
	operatorFormat.setForeground(QColor("green"));
	operatorFormat.setFontItalic(true);
	QStringList operatorPatterns;
	operatorPatterns << "\\b+\\b" ; //<< "\\b\\-\\b" << "\\b\\*\\b" << "\\b\\!\\b"<< "\\b\\%\\b"<< "\\b\\&\\b" << "\\b\\/\\b";
	foreach (const QString &pattern, operatorPatterns) {
		rule.pattern = QRegularExpression(pattern);
		rule.format = operatorFormat;
		highlightingRules.append(rule);
	}
	*/

	//Paren
	parenFormat.setForeground(QColor("#ffffff"));
	QStringList parenPatterns;
	parenPatterns << "\\b\\{\\b" << "\\b\\}\\b"  << "\\b\\[\\b" << "\\b\\]\\b" << "\\b\\(\\b" << "\\b\\)\\b";
	foreach (const QString &pattern, parenPatterns) {
		highlightingRules.append(HighlightingRule (QRegularExpression(pattern), parenFormat));
	}

	//Keyword
	keywordFormat.setForeground(QColor("#e9d000"));
	keywordFormat.setFontWeight(QFont::Bold);
	QStringList keywordPatterns;
	keywordPatterns << "\\bplan\\b" << "\\bpuppet\\b" << "\\bmember\\b" << "\\blimb\\b";
	foreach (const QString &pattern, keywordPatterns) {
		highlightingRules.append(HighlightingRule (QRegularExpression(pattern), keywordFormat));
	}




	//Hub
	hubFormat.setForeground(QColor("#45de47"));
	hubFormat.setFontWeight(QFont::Bold);
	highlightingRules.append(HighlightingRule (QRegularExpression("\\bhub\\b"), hubFormat));

	//Remote
	remoteFormat.setForeground(QColor("#2faad0"));
	remoteFormat.setFontWeight(QFont::Bold);
	highlightingRules.append(HighlightingRule (QRegularExpression("\\bremote\\b"), remoteFormat));

	//Agent
	agentFormat.setForeground(QColor("#da3333"));
	agentFormat.setFontWeight(QFont::Bold);
	highlightingRules.append(HighlightingRule (QRegularExpression("\\bagent\\b"), agentFormat));

	operatorFormat.setFontWeight(QFont::Bold);
	operatorFormat.setForeground(QColor("#d0e900"));
	highlightingRules.append(HighlightingRule (QRegularExpression("\\b[\\\\+" + QRegularExpression::escape("e+-*/!=<>")+ "]+\\b"), operatorFormat));

	classFormat.setFontWeight(QFont::Bold);
	classFormat.setForeground(QColor("#d0e900"));
	highlightingRules.append(HighlightingRule (QRegularExpression("\\bQ[A-Za-z]+\\b"), classFormat));

	singleLineCommentFormat.setForeground(QColor("#666666"));
	highlightingRules.append(HighlightingRule (QRegularExpression("//[^\n]*"), singleLineCommentFormat));
	commentStartExpression = QRegularExpression("/\\*");
	commentEndExpression = QRegularExpression("\\*/");
	multiLineCommentFormat.setForeground(QColor("#666666"));

	stringFormat.setForeground(QColor("#00d0e9"));
	highlightingRules.append(HighlightingRule (QRegularExpression("\".*\""), stringFormat));

	functionFormat.setFontItalic(true);
	functionFormat.setForeground(QColor("#00e9d0"));
	highlightingRules.append(HighlightingRule (QRegularExpression("\\b[A-Za-z0-9_]+(?=\\()"), functionFormat));


	// Optimize
	foreach (const HighlightingRule &rule, highlightingRules) {
		rule.pattern.optimize();
	}


}

void MarkdownHighlighter::highlightBlock(const QString &text){
	for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
		QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
		while (matchIterator.hasNext()) {
			QRegularExpressionMatch match = matchIterator.next();
			setFormat(match.capturedStart(), match.capturedLength(), rule.format);
		}
	}
	setCurrentBlockState(0);

	int startIndex = 0;
	if (previousBlockState() != 1){
		startIndex = text.indexOf(commentStartExpression);
		while (startIndex >= 0) {
			QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
			int endIndex = match.capturedStart();
			int commentLength = 0;
			if (endIndex == -1) {
				setCurrentBlockState(1);
				commentLength = text.length() - startIndex;
			} else {
				commentLength = endIndex - startIndex + match.capturedLength();
			}
			setFormat(startIndex, commentLength, multiLineCommentFormat);
			startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
		}
	}
}











































