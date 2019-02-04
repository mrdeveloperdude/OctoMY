#include "Markdown.hpp"


#ifdef OC_USE_FEATURE_MARKDOWN

//extern "C" {
	#include "markdown/document.h"
	#include "markdown/html.h"
	#include "markdown/buffer.h"

//}

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
	hoedown_html_flags html_flags = static_cast<hoedown_html_flags>(0); // Cast means we want empty flags enum
	int toc_level=0;
	hoedown_renderer *renderer = hoedown_html_renderer_new(html_flags, toc_level);
	hoedown_buffer *ob = hoedown_buffer_new(DEF_OUNIT);
	hoedown_buffer *ib = hoedown_buffer_new(DEF_IUNIT);

	hoedown_extensions extensions = static_cast<hoedown_extensions>(0); // Cast means we want empty flags enum
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
