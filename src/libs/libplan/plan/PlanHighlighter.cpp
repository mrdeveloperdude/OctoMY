#include "PlanHighlighter.hpp"

#include "utility/ui/Style.hpp"
#include "app/Constants.hpp"



PlanHighlighter::PlanHighlighter(QTextDocument *parent)
	: QSyntaxHighlighter(parent)
{
	//utility::color::textCharFormat(QColor foregroundColor, QColor backgroundColor, bool bold, bool underline, bool strikeout, bool italic);
	utility::ui::HighlightingPalette palette(
				// Base
				utility::ui::textCharFormat(QColor(0x222222))
				// Operator
				, utility::ui::textCharFormat(QColor(0xd0e900), Qt::transparent, true)
				//Paren
				, utility::ui::textCharFormat(QColor(0xffffff))
				//Keyword
				, utility::ui::textCharFormat(QColor(0xe9d000))
				//Hub
				, utility::ui::textCharFormat(Constants::OC_NODE_TYPE_HUB_COLOR, Qt::transparent, true)
				//Agent
				, utility::ui::textCharFormat(Constants::OC_NODE_TYPE_AGENT_COLOR, Qt::transparent, true)
				//Remote
				, utility::ui::textCharFormat(Constants::OC_NODE_TYPE_REMOTE_COLOR, Qt::transparent, true)
				//Class
				, utility::ui::textCharFormat(QColor(0xd0e900), Qt::transparent, true)
				//Single line comment
				, utility::ui::textCharFormat(QColor(0x666666))
				// Multi line comment
				, utility::ui::textCharFormat(QColor(0x666666))
				// String literal
				, utility::ui::textCharFormat(QColor(0x00d0e9))
				// Number literal
				, utility::ui::textCharFormat(QColor(0x000001))
				// Function
				, utility::ui::textCharFormat(QColor(0x00e9d0), Qt::transparent, false, false, false, true)
				);
	
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
	QStringList parenPatterns;
	parenPatterns << "\\b\\{\\b" << "\\b\\}\\b"  << "\\b\\[\\b" << "\\b\\]\\b" << "\\b\\(\\b" << "\\b\\)\\b";
	foreach (const QString &pattern, parenPatterns) {
		highlightingRules.append(HighlightingRule (QRegularExpression(pattern), palette.parenColor));
	}
	//Keyword
	QStringList keywordPatterns;
	keywordPatterns << "\\bplan\\b" << "\\bpuppet\\b" << "\\bmember\\b" << "\\blimb\\b";
	foreach (const QString &pattern, keywordPatterns) {
		highlightingRules.append(HighlightingRule (QRegularExpression(pattern), palette.keywordColor));
	}
	//Hub
	highlightingRules.append(HighlightingRule (QRegularExpression("\\bhub\\b"), palette.hubColor ));
	//Remote
	highlightingRules.append(HighlightingRule (QRegularExpression("\\bremote\\b"), palette.remoteColor ));
	//Agent
	highlightingRules.append(HighlightingRule (QRegularExpression("\\bagent\\b"), palette.agentColor));
	// Operator
	highlightingRules.append(HighlightingRule (QRegularExpression("\\b[\\\\+" + QRegularExpression::escape("e+-*/!=<>")+ "]+\\b"), palette.operatorColor ));
	// Class
	highlightingRules.append(HighlightingRule (QRegularExpression("\\bQ[A-Za-z]+\\b"), palette.classColor ));
	// Single line comment
	highlightingRules.append(HighlightingRule (QRegularExpression("//[^\n]*"), palette.singleLineCommentColor ));
	// Multi line comment
	commentStartExpression = QRegularExpression("/\\*");
	commentEndExpression = QRegularExpression("\\*/");
	multiLineCommentFormat = palette.multiLineCommentColor;
	// String literal
	highlightingRules.append(HighlightingRule (QRegularExpression("\".*\""), palette.stringLiteralColor ));
	// Number literal
	//highlightingRules.append(HighlightingRule (QRegularExpression("\".*\""), palette.numberLiteral ));
	// function
	highlightingRules.append(HighlightingRule (QRegularExpression("\\b[A-Za-z0-9_]+(?=\\()"), palette.functionColor ));
	// Optimize
	foreach (const HighlightingRule &rule, highlightingRules) {
		rule.pattern.optimize();
	}
}

void PlanHighlighter::highlightBlock(const QString &text){
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


