#ifndef UTILITY_STYLE_HPP
#define UTILITY_STYLE_HPP


#include <QTextCharFormat>


namespace utility{

namespace ui{




inline QTextCharFormat textCharFormat(QColor foregroundColor, QColor backgroundColor=Qt::transparent, bool bold=false, bool underline=false, bool italic=false, bool strikeout=false, bool spellCheck=false);

QTextCharFormat textCharFormat(QColor foregroundColor, QColor backgroundColor, bool bold, bool underline, bool strikeout, bool italic, bool spellCheck){
	QTextCharFormat f;
	f.setFontItalic(italic);
	
	f.setFontStrikeOut(strikeout);
	f.setFontWeight(bold?QFont::Bold:QFont::Normal);
	f.setForeground(foregroundColor);
	f.setBackground(backgroundColor);
	if(spellCheck){
		f.setUnderlineColor(Qt::red);
		f.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
		f.setFontUnderline(true);
	} else {
		f.setFontUnderline(underline);
	}
	return f;
}



struct HighlightingPalette{
	QTextCharFormat baseColor;
	QTextCharFormat operatorColor;
	QTextCharFormat parenColor;
	QTextCharFormat keywordColor;
	QTextCharFormat hubColor;
	QTextCharFormat agentColor;
	QTextCharFormat remoteColor;
	QTextCharFormat classColor;
	QTextCharFormat singleLineCommentColor;
	QTextCharFormat multiLineCommentColor;
	QTextCharFormat stringLiteralColor;
	QTextCharFormat numberLiteralColor;
	QTextCharFormat functionColor;
	QTextCharFormat headingColor;
	QTextCharFormat quoteColor;
	QTextCharFormat codeColor;
	QTextCharFormat tableColor;
	QTextCharFormat listColor;
	QTextCharFormat linkColor;
	HighlightingPalette(
			QTextCharFormat baseColor
			, QTextCharFormat operatorColor = QTextCharFormat()
			, QTextCharFormat parenColor = QTextCharFormat()
			, QTextCharFormat keywordColor = QTextCharFormat()
			, QTextCharFormat hubColor = QTextCharFormat()
			, QTextCharFormat agentColor = QTextCharFormat()
			, QTextCharFormat remoteColor = QTextCharFormat()
			, QTextCharFormat classColor = QTextCharFormat()
			, QTextCharFormat singleLineCommentColor = QTextCharFormat()
			, QTextCharFormat multiLineCommentColor = QTextCharFormat()
			, QTextCharFormat stringLiteralColor = QTextCharFormat()
			, QTextCharFormat numberLiteralColor = QTextCharFormat()
			, QTextCharFormat functionColor = QTextCharFormat()
			, QTextCharFormat headingColor = QTextCharFormat()
			, QTextCharFormat quoteColor = QTextCharFormat()
			, QTextCharFormat codeColor = QTextCharFormat()
			, QTextCharFormat tableColor = QTextCharFormat()
			, QTextCharFormat listColor = QTextCharFormat()
			, QTextCharFormat linkColor = QTextCharFormat()
			
			):
		baseColor(baseColor)
	  , operatorColor(operatorColor)
	  , parenColor(parenColor)
	  , keywordColor(keywordColor)
	  , hubColor(hubColor)
	  , agentColor(agentColor)
	  , remoteColor(remoteColor)
	  , classColor(classColor)
	  , singleLineCommentColor(singleLineCommentColor)
	  , multiLineCommentColor(multiLineCommentColor)
	  , stringLiteralColor(stringLiteralColor)
	  , numberLiteralColor(numberLiteralColor)
	  , functionColor(functionColor)
	  , headingColor(headingColor)
	  , quoteColor(quoteColor)
	  , codeColor(codeColor)
	  , tableColor(tableColor)
	  , listColor(listColor)
	  , linkColor(linkColor)
	{
		
	}
};




}
}



#endif
// UTILITY_STYLE_HPP
