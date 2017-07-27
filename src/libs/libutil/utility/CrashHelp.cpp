#include "CrashHelp.hpp"

#include <qsystemdetection.h>
#include <QDebug>

//TODO: Look at https://stackoverflow.com/questions/8115192/android-ndk-getting-the-backtrace to have this working on more platforms
#if defined(__GNUC__) && !defined(__CYGWIN__) && !defined(Q_OS_ANDROID)

#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <typeinfo>
#include <cxxabi.h>

#include <QString>
#include <QDebug>



static QString demangleFrame(const char *symbol)
{
	size_t size;
	int status;
	char temp[128];
	char* result;

	if (1 == sscanf(symbol, "%*[^'(']%*[^'_']%[^')''+']", temp)) {
		if (NULL != (result = abi::__cxa_demangle(temp, NULL, &size, &status))) {
			QString r = result;
			return r + " [" + symbol + "]";
		}
	}

	if (1 == sscanf(symbol, "%127s", temp)) {
		return temp;
	}

	return QString::fromLatin1(symbol,strlen(symbol));
}

static void terminationHandler(void)
{
	qWarning()<<"CRASH HELP DETECTED Unhandeled ERROR ########";
	void *array[32];
	size_t size = backtrace(array, 32);
	char **strings = backtrace_symbols(array, size);
	qWarning()<< "CRASH HELP Obtained '" << size << "' frames:";
	for (size_t i = 0; i < size; i++) {
		qWarning()<<"CRASH HELP  + " << demangleFrame(strings[i]);
	}
	free(strings);
}
#endif

CrashHelp::CrashHelp()
{
}


CrashHelp::CrashHelp(bool enableImplicitly)
{
	if(enableImplicitly) {
		setEnabled(true);
	}
}


CrashHelp::~CrashHelp()
{
	setEnabled(false);
}


void CrashHelp::setEnabled(bool enabled)
{

#if defined(__GNUC__) && !defined(__CYGWIN__) && !defined(Q_OS_ANDROID)

	std::set_terminate(enabled?terminationHandler:nullptr);
	qDebug().nospace().noquote()<<"CRASH HELP WAS "<<(enabled?"INSTALLED":"UN-INSTALLED");

#else
	qDebug().nospace().noquote()<<"CRASH HELP WAS NOT "<<(enabled?"INSTALLED":"UN-INSTALLED");


#endif


}
