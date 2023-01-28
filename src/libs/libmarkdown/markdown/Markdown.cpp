#include "Markdown.hpp"


#ifdef OC_USE_FEATURE_MARKDOWN

#include "markdown/document.h"
#include "markdown/html.h"
#include "markdown/buffer.h"

#define DEF_IUNIT 1024
#define DEF_OUNIT 64
#define DEF_MAX_NESTING 16


Markdown::Markdown()
{

}

bool Markdown::isAvailable()
{
	return true;
}


QString Markdown::process(QString input)
{
	
	auto all_extensions= //0b11111111111111111111111111111111;
			
			0
			| HOEDOWN_EXT_TABLES 
			| HOEDOWN_EXT_FENCED_CODE
			| HOEDOWN_EXT_FOOTNOTES
			| HOEDOWN_EXT_AUTOLINK
			| HOEDOWN_EXT_STRIKETHROUGH
			| HOEDOWN_EXT_UNDERLINE
			| HOEDOWN_EXT_HIGHLIGHT
			| HOEDOWN_EXT_QUOTE
			| HOEDOWN_EXT_SUPERSCRIPT
			| HOEDOWN_EXT_MATH
			| HOEDOWN_EXT_NO_INTRA_EMPHASIS
			| HOEDOWN_EXT_SPACE_HEADERS
			| HOEDOWN_EXT_MATH_EXPLICIT
			| HOEDOWN_EXT_DISABLE_INDENTED_CODE ;


			
	
	/*
	 //block-level extensions 
	HOEDOWN_EXT_TABLES = (1 << 0),
	HOEDOWN_EXT_FENCED_CODE = (1 << 1),
	HOEDOWN_EXT_FOOTNOTES = (1 << 2),
	
	// span-level extensions 
	HOEDOWN_EXT_AUTOLINK = (1 << 3),
	HOEDOWN_EXT_STRIKETHROUGH = (1 << 4),
	HOEDOWN_EXT_UNDERLINE = (1 << 5),
	HOEDOWN_EXT_HIGHLIGHT = (1 << 6),
	HOEDOWN_EXT_QUOTE = (1 << 7),
	HOEDOWN_EXT_SUPERSCRIPT = (1 << 8),
	HOEDOWN_EXT_MATH = (1 << 9),

	// other flags
	HOEDOWN_EXT_NO_INTRA_EMPHASIS = (1 << 11),
	HOEDOWN_EXT_SPACE_HEADERS = (1 << 12),
	HOEDOWN_EXT_MATH_EXPLICIT = (1 << 13),

	// negative flags 
	HOEDOWN_EXT_DISABLE_INDENTED_CODE = (1 << 14)
*/
	
	

	
			
	hoedown_html_flags html_flags = static_cast<hoedown_html_flags>(0);
	int toc_level=0;
	hoedown_renderer *renderer = hoedown_html_renderer_new(html_flags, toc_level);
	hoedown_buffer *ob = hoedown_buffer_new(DEF_OUNIT);
	hoedown_buffer *ib = hoedown_buffer_new(DEF_IUNIT);

	hoedown_extensions extensions = static_cast<hoedown_extensions>(all_extensions);
	size_t max_nesting=DEF_MAX_NESTING;

	auto data=reinterpret_cast<const unsigned char *>(input.toLocal8Bit().constData());
	hoedown_buffer_set(ib, data, static_cast<size_t>(input.size()));

	hoedown_document *document = hoedown_document_new(renderer, extensions, max_nesting);
	hoedown_document_render(document, ob, ib->data, ib->size);

	hoedown_buffer_free(ib);
	hoedown_document_free(document);
	hoedown_html_renderer_free(renderer);

	QString output=QString::fromLocal8Bit(  reinterpret_cast<const char *>(ob->data), static_cast<int>(ob->size) );
	hoedown_buffer_free(ob);
	return output;
}

#else






// No-op
Markdown::Markdown()
{
}

bool Markdown::isAvailable()
{
	return false;
}

QString Markdown::process(QString input){
	return "";
}

#endif
