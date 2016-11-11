#include <QtGlobal>
//#include "QtIconv.h"

/*
QtIconv is an attempt to emulate the functionality of iconv using only Qt.

Why? Because while iconv is awesome in its own right, I have found that
embedding a library wich depends on iconv into my Qt only project adds a lot of
unwanted complexity to the build procedure. While my project shall be buildable
simply by clicking the green play button in QtCreator, having iconv as a
dependency requires a separate "download + configure + make + make install" step
that simply is not welcome in the project.

Since the project is already based on Qt and since Qt has the functionality
provided by iconv built into it, I decided to see if I could emulate iconv and
thus make the dependence on iconv go away.

*/




#include <QString>
#include <QByteArray>
#include <QTextCodec>
#include <QDebug>


extern "C" typedef void *iconv_t;


struct QtIconv{
		QTextCodec *fromCodec = 0;
		QTextCodec *toCodec = 0;
		QtIconv( const char *fromEncoding, const char *toEncoding)
			: fromCodec(QTextCodec::codecForName(fromEncoding))
			, toCodec(QTextCodec::codecForName(toEncoding))
		{

		}

};


extern "C" iconv_t iconv_open (const char *__tocode, const char *__fromcode)
{
	QtIconv *qt_iconv=new QtIconv(__tocode, __fromcode);
	return (iconv_t)qt_iconv;
}

extern "C" size_t iconv (iconv_t __cd, char **__restrict __inbuf, size_t *__restrict __inbytesleft, char **__restrict __outbuf, size_t *__restrict __outbytesleft)
{
	//Being a rush project, this function cannot afford not to be defensive, so
	//every small indiscrepancy results in failure.
	QtIconv *qt_iconv=(QtIconv *)__cd;
	if(0==qt_iconv){
		qWarning()<<"qt_iconv was null";
		return 0;
	}
	if(0==qt_iconv->fromCodec){
		qWarning()<<"qt_iconv->fromCodec was null";
		return 0;
	}
	if(0==qt_iconv->toCodec){
		qWarning()<<"qt_iconv->toCodec was null";
		return 0;
	}
	if(0==__inbuf){
		qWarning()<<"__inbuf was null";
		return 0;
	}
	if(0==__outbuf){
		qWarning()<<"__outbuf was null";
		return 0;
	}
	char *_inbuf=__inbuf[0];
	char *_outbuf=__outbuf[0];
	if(0==_inbuf){
		qWarning()<<"_inbuf was null";
		return 0;
	}
	if(0==_outbuf){
		qWarning()<<"_outbuf was null";
		return 0;
	}
	if(0==__outbytesleft){
		qWarning()<<"__outbytesleft was null";
		return 0;
	}
	size_t _outbytesleft=__outbytesleft[0];
	if(0==_outbytesleft){
		qWarning()<<"_outbytesleft was null";
		return 0;
	}
	if(0==__inbytesleft){
		qWarning()<<"__inbytesleft was null";
		return 0;
	}
	size_t _inbytesleft=__inbytesleft[0];
	if(0==_inbytesleft){
		qWarning()<<"_inbytesleft was null";
		return 0;
	}
	QByteArray from_text(_inbuf, _inbytesleft);
	QByteArray to_text= qt_iconv->toCodec->fromUnicode(qt_iconv->fromCodec->toUnicode(from_text));
	size_t outsize=to_text.size();
	if(outsize>_outbytesleft){
		outsize=_outbytesleft;
	}
	memcpy(_outbuf, to_text.data(), outsize);
	//WARNING: Gross simplifaction that simlpy says "everything was converted" even if that was not the case.
	__inbytesleft[0]=0;
	__outbytesleft[0]=_outbytesleft-outsize;
	return outsize;
}


extern "C" int iconv_close (iconv_t __cd)
{
	QtIconv *qt_iconv=(QtIconv *)__cd;
	delete qt_iconv;
	qt_iconv=0;
	return 0;
}
