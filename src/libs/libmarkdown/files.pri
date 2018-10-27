MARKDOWN_TOP= $${SRCS}/libs/libmarkdown

# From https://github.com/hoedown/hoedown

contains(DEFINES, USE_ZBAR){

HEADERS	+= \
	$$MARKDOWN_TOP/markdown/autolink.h \
	$$MARKDOWN_TOP/markdown/buffer.h \
	$$MARKDOWN_TOP/markdown/document.h \
	$$MARKDOWN_TOP/markdown/escape.h \
	$$MARKDOWN_TOP/markdown/html.h \
	$$MARKDOWN_TOP/markdown/stack.h \
	$$MARKDOWN_TOP/markdown/version.h \


SOURCES	+= \
	$$MARKDOWN_TOP/markdown/autolink.c \
	$$MARKDOWN_TOP/markdown/buffer.c \
	$$MARKDOWN_TOP/markdown/document.c \
	$$MARKDOWN_TOP/markdown/escape.c \
	$$MARKDOWN_TOP/markdown/html_blocks.c \
	$$MARKDOWN_TOP/markdown/html.c \
	$$MARKDOWN_TOP/markdown/html_smartypants.c \
	$$MARKDOWN_TOP/markdown/stack.c \
	$$MARKDOWN_TOP/markdown/version.c \


RESOURCES += \

}

#NOTE: These are always included to keep the customers of this lib happy

HEADERS	+= \
	$$MARKDOWN_TOP/markdown/Markdown.hpp

SOURCES	+= \
	$$MARKDOWN_TOP/markdown/Markdown.cpp
