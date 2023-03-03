#include "HTMLHighlighter.hpp"

#include <QTextCharFormat>
#include <QBrush>
#include <QColor>

// Qt6 HTML QSyntaxHighlighter highlighter for QTextDocument.
// Uses QRegularExpression instead of QRegExp

HTMLHighlighter::HTMLHighlighter(QTextDocument *parent)
	: QSyntaxHighlighter(parent)
	, openTag("<")
	, closeTag(">")
	, commentStartExpression("<!--")
	, commentEndExpression("-->")
	, quotes("\"")
{
	QStringList tags;
	tags
			<< "a"
			<< "abbr"
			<< "acronym"
			<< "address"
			<< "amenuitem"
			<< "applet"
			<< "area"
			<< "article"
			<< "aside"
			<< "audio"
			<< "b"
			<< "base"
			<< "basefont"
			<< "bdi"
			<< "bdo"
			<< "bgsound"
			<< "big"
			<< "blink"
			<< "blockquote"
			<< "body"
			<< "br"
			<< "button"
			<< "canvas"
			<< "caption"
			<< "center"
			<< "cite"
			<< "code"
			<< "col"
			<< "colgroup"
			<< "command"
			<< "comment"
			<< "data"
			<< "datalist"
			<< "dd"
			<< "del"
			<< "details"
			<< "dfn"
			<< "dialog"
			<< "dir"
			<< "div"
			<< "dl"
			<< "dt"
			<< "em"
			<< "embed"
			<< "fieldset"
			<< "figcaption"
			<< "figure"
			<< "font"
			<< "footer"
			<< "form"
			<< "frame"
			<< "frameset"
			<< "h1"
			<< "h2"
			<< "h3"
			<< "h4"
			<< "h5"
			<< "h6"
			<< "head"
			<< "header"
			<< "hgroup"
			<< "hr"
			<< "html"
			<< "i"
			<< "iframe"
			<< "img"
			<< "input"
			<< "ins"
			<< "isindex"
			<< "kbd"
			<< "keygen"
			<< "label"
			<< "legend"
			<< "li"
			<< "link"
			<< "listing"
			<< "main"
			<< "map"
			<< "mark"
			<< "marquee"
			<< "menu"
			<< "meta"
			<< "meter"
			<< "multicol"
			<< "nav"
			<< "nobr"
			<< "noembed"
			<< "noframes"
			<< "noindex"
			<< "noscript"
			<< "object"
			<< "ol"
			<< "optgroup"
			<< "option"
			<< "output"
			<< "p"
			<< "param"
			<< "picture"
			<< "plaintext"
			<< "pre"
			<< "progress"
			<< "q"
			<< "rp"
			<< "rt"
			<< "rtc"
			<< "ruby"
			<< "s"
			<< "samp"
			<< "script"
			<< "section"
			<< "select"
			<< "small"
			<< "source"
			<< "spacer"
			<< "span"
			<< "strike"
			<< "strong"
			<< "style"
			<< "sub"
			<< "summary"
			<< "sup"
			<< "table"
			<< "tbody"
			<< "td"
			<< "template"
			<< "textarea"
			<< "tfoot"
			<< "th"
			<< "thead"
			<< "time"
			<< "title"
			<< "tr"
			<< "track"
			<< "tt"
			<< "u"
			<< "ul"
			<< "var"
			<< "video"
			<< "wbr"
			<< "xmp";
	
	
	HighlightingRule rule;
	
	edgeTagFormat.setForeground(QBrush(QColor(0x32a9dd)));
	insideTagFormat.setForeground(Qt::blue);
	insideTagFormat.setFontWeight(QFont::Bold);
	
	tagsFormat.setForeground(Qt::darkBlue);
	tagsFormat.setFontWeight(QFont::Bold);
	
	
	for (const QString &tag : tags)
	{
		QString openPattern{"<\\b"+tag+"\\b"};
		QString closePattern{"</\\b"+tag+"\\b"};
		rule.pattern = QRegularExpression(openPattern);
		rule.pattern.optimize();
		rule.format = tagsFormat;
		startTagRules.append(rule);
		rule.pattern = QRegularExpression(closePattern);
		rule.pattern.optimize();
		rule.format = tagsFormat;
		endTagRules.append(rule);
	}
	multiLineCommentFormat.setForeground(Qt::darkGray);
	quotationFormat.setForeground(Qt::darkGreen);
}


void HTMLHighlighter::highlightBlock(const QString &text)
{
	setCurrentBlockState(HTMLHighlighter::None);
	
	// TAG
	int startIndex = 0;
	// If you're not within a tag,
	if (previousBlockState() != HTMLHighlighter::Tag && previousBlockState() != HTMLHighlighter::Quote)
	{
		// So we try to find the beginning of the next tag
		//startIndex = openTag.indexIn(text);
		startIndex = openTag.match(text).capturedStart();
	}
	
	// Taking the state of the previous text block
	int subPreviousTag = previousBlockState();
	while (startIndex >= 0)
	{
		// We are looking for an end-tag
		//int endIndex = closeTag.indexIn(text, startIndex);
		int endIndex = closeTag.match(text, startIndex).capturedStart();
		
		int tagLength;
		// If the end tag is not found, then we set the block state
		if (endIndex == -1)
		{
			setCurrentBlockState(HTMLHighlighter::Tag);
			tagLength = text.length() - startIndex;
		}
		else
		{
			//tagLength = endIndex - startIndex + closeTag.matchedLength();
			tagLength = endIndex - startIndex + closeTag.match(text).capturedLength();
		}
		
		// Set the formatting for a tag
		if (subPreviousTag != HTMLHighlighter::Tag)
		{
			// since the beginning of the tag to the end, if the previous status is not equal Tag
			setFormat(startIndex, 1, edgeTagFormat);
			setFormat(startIndex + 1, tagLength - 1, insideTagFormat);
		}
		else
		{
			// If you're already inside the tag from the start block
			// and before the end tag
			setFormat(startIndex, tagLength, insideTagFormat);
			subPreviousTag = HTMLHighlighter::None;
		}
		// Format the symbol of the end tag
		setFormat(endIndex, 1, edgeTagFormat);
		
		/// QUOTES ///////////////////////////////////////
		int startQuoteIndex = 0;
		// If you are not in quotation marks with the previous block
		if (previousBlockState() != HTMLHighlighter::Quote)
		{
			// So we try to find the beginning of the quotes
			//startQuoteIndex = quotes.indexIn(text, startIndex);
			startQuoteIndex = quotes.match(text, startIndex).capturedStart();
		}
		
		// Highlights all quotes within the tag
		while (startQuoteIndex >= 0 && ((startQuoteIndex < endIndex) || (endIndex == -1)))
		{
			//int endQuoteIndex = quotes.indexIn(text, startQuoteIndex + 1);
			int endQuoteIndex = quotes.match(text, startQuoteIndex + 1).capturedStart();
			
			int quoteLength;
			if (endQuoteIndex == -1)
			{
				// If a closing quotation mark is found, set the state for the block Quote
				setCurrentBlockState(HTMLHighlighter::Quote);
				quoteLength = text.length() - startQuoteIndex;
			}
			else
			{
				//quoteLength = endQuoteIndex - startQuoteIndex + quotes.matchedLength();
				quoteLength = endQuoteIndex - startQuoteIndex + quotes.match(text).capturedLength();
			}
			
			if ((endIndex > endQuoteIndex) || endIndex == -1)
			{
				setFormat(startQuoteIndex, quoteLength, quotationFormat);
				//startQuoteIndex = quotes.indexIn(text, startQuoteIndex + quoteLength);
				startQuoteIndex = quotes.match(text, startQuoteIndex + quoteLength).capturedStart();
			}
			else
			{
				break;
			}
		}
		//////////////////////////////////////////////////
		// Again, look for the beginning of the tag
		//startIndex = openTag.indexIn(text, startIndex + tagLength);
		startIndex = openTag.match(text, startIndex + tagLength).capturedStart();
	}
	
	// EDGES OF TAGS
	// Processing of color tags themselves, that is, highlight words div, p, strong etc.
	for (const HighlightingRule &rule : startTagRules)
	{
		auto &expression=rule.pattern;
		//int index = expression.indexIn(text);
		int index = expression.match(text).capturedStart();
		
		
		while (index >= 0)
		{
			//int length = expression.matchedLength();
			int length = expression.match(text).capturedLength();
			setFormat(index + 1, length - 1, rule.format);
			//index = expression.indexIn(text, index + length);
			index = expression.match(text, index + length).capturedStart();
		}
	}
	
	for (const HighlightingRule &rule : endTagRules)
	{
		auto &expression=rule.pattern;
		//int index = expression.indexIn(text);
		int index = expression.match(text).capturedStart();
		while (index >= 0) {
			//int length = expression.matchedLength();
			int length = expression.match(text).capturedLength();
			setFormat(index + 1, 1, edgeTagFormat);
			setFormat(index + 2, length - 2, rule.format);
			//index = expression.indexIn(text, index + length);
			index = expression.match(text, index + length).capturedStart();
		}
	}
	
	// COMMENT
	int startCommentIndex = 0;
	// If the tag is not a previous state commentary
	if (previousBlockState() != HTMLHighlighter::Comment)
	{
		// then we try to find the beginning of a comment
		//startCommentIndex = commentStartExpression.indexIn(text);
		startCommentIndex = commentStartExpression.match(text).capturedStart();
	}
	
	// If a comment is found
	while (startCommentIndex >= 0)
	{
		// We are looking for the end of the comment
		//int endCommentIndex = commentEndExpression.indexIn(text, startCommentIndex);
		int endCommentIndex = commentEndExpression.match(text, startCommentIndex).capturedStart();
		int commentLength;
		
		// If the end is not found
		if (endCommentIndex == -1)
		{
			// That set the state Comment
			// The principle is similar to that for conventional tags
			setCurrentBlockState(HTMLHighlighter::Comment);
			commentLength = text.length() - startCommentIndex;
		}
		else
		{
			//commentLength = endCommentIndex - startCommentIndex + commentEndExpression.matchedLength();
			commentLength = endCommentIndex - startCommentIndex + commentEndExpression.match(text).capturedLength();
			
		}
		setFormat(startCommentIndex, commentLength, multiLineCommentFormat);
		//startCommentIndex = commentStartExpression.indexIn(text, startCommentIndex + commentLength);
		startCommentIndex = commentEndExpression.match(text, startCommentIndex + commentLength).capturedStart();
		
	}
}
