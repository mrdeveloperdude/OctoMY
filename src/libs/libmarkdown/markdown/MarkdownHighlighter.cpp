#include "MarkdownHighlighter.hpp"

#include "utility/ui/Style.hpp"
#include "app/Constants.hpp"

MarkdownHighlighter::MarkdownHighlighter(QTextDocument *parent)
	: QSyntaxHighlighter(parent)
{
	//utility::color::textCharFormat(QColor foregroundColor, QColor backgroundColor, bool bold, bool underline, bool strikeout, bool italic);
	utility::ui::HighlightingPalette palette(
				// Base
				utility::ui::textCharFormat(QColor(0x222222))
				// Operator
				, utility::ui::textCharFormat(QColor(0xd0e900), Qt::transparent, true)
				// Paren
				, utility::ui::textCharFormat(QColor(0xffffff))
				// Keyword
				, utility::ui::textCharFormat(QColor(0xe9d000))
				// Hub
				, utility::ui::textCharFormat(Constants::OC_NODE_TYPE_HUB_COLOR, Qt::transparent, true)
				// Agent
				, utility::ui::textCharFormat(Constants::OC_NODE_TYPE_AGENT_COLOR, Qt::transparent, true)
				// Remote
				, utility::ui::textCharFormat(Constants::OC_NODE_TYPE_REMOTE_COLOR, Qt::transparent, true)
				// Class
				, utility::ui::textCharFormat(QColor(0xd0e900), Qt::transparent, true)
				// Single line comment
				, utility::ui::textCharFormat(QColor(0x666666))
				// Multi line comment
				, utility::ui::textCharFormat(QColor(0x666666))
				// String literal
				, utility::ui::textCharFormat(QColor(0x00d0e9))
				// Number literal
				, utility::ui::textCharFormat(QColor(0x000001))
				// Function
				, utility::ui::textCharFormat(QColor(0x00e9d0), Qt::transparent, false, false, false, true)
				
				// Heading
				, utility::ui::textCharFormat(QColor(0xff0000), Qt::transparent, true)
				
				// Quote
				, utility::ui::textCharFormat(QColor(0xff8888), Qt::transparent, true)
				
				// Code
				, utility::ui::textCharFormat(QColor(0xffff00), Qt::transparent, true)
				
				// Table
				, utility::ui::textCharFormat(QColor(0x88ff88), Qt::transparent, true)

				// List
				, utility::ui::textCharFormat(QColor(0x0000ff), Qt::transparent, true)
				
				// Link
				, utility::ui::textCharFormat(QColor(0x8800ff), Qt::transparent, true)
				);


	// Heading
	highlightingRules.append(HighlightingRule (QRegularExpression("^[#]+ [^\n]*"), palette.headingColor ));
	highlightingRules.append(HighlightingRule (QRegularExpression("^[=]+\n"), palette.headingColor ));
	highlightingRules.append(HighlightingRule (QRegularExpression("^[\\-]+\n"), palette.headingColor ));
	
	// Tables
	highlightingRules.append(HighlightingRule (QRegularExpression("^\\|[^\n]*"), palette.tableColor ));
	
	// Block quotes
	highlightingRules.append(HighlightingRule (QRegularExpression("^\\> [^\n]*"), palette.quoteColor ));
	
	// Lists
	highlightingRules.append(HighlightingRule (QRegularExpression("^[ ]?[\\*\\+\\-] [^\n]*"), palette.listColor ));
	highlightingRules.append(HighlightingRule (QRegularExpression("^[ ]?\\.[0-9]* [^\n]*"), palette.listColor ));
	
	// Links
	highlightingRules.append(HighlightingRule (QRegularExpression("\\[.*\\]\\w*\\(.*\\)"), palette.linkColor ));
	highlightingRules.append(HighlightingRule (QRegularExpression("^\\[[1-9][0-9]*\\]: [^\"]*\\w*\"[^\"]*\"\n "), palette.linkColor ));
	highlightingRules.append(HighlightingRule (QRegularExpression("\\[[^\\]]*\\][\\w]*\\[[1-9][0-9]*\\]" ), palette.linkColor ));
	
	// Code
	highlightingRules.append(HighlightingRule (QRegularExpression("(?<!\\)`.*`"), palette.codeColor ));
	highlightingRules.append(HighlightingRule (QRegularExpression("^\t[^\n]*"), palette.codeColor ));
	
	
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
}











































