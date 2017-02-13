#ifndef ARDUMY_HPP
#define ARDUMY_HPP

// Allow inclusion of sources
// + without dependency on Arduino for Qt builds
// + without dependence on Qt for Arduino builds


// Why? Because building for Qt makes it possible to write all sorts of advanced and resource hungry instrumentation and testing code and debugging output not limited by Serial.print*
// While Arduino is the target platform

#ifdef ARDUINO_ARCH_AVR

#if ARDUINO >= 100
#include <Arduino.h>
#elif ARDUINO < 100
#include <WProgram.h>
#else
#warning "NO ARDUINO VERSION PRESENT"
#endif

#define Q_ASSERT(A)
#define Q_CHECK_PTR(A)
#define qreal float


struct QDebugOutput {

	static QDebugOutput *d;
	static QDebugOutput *w;

	static QDebugOutput &debug(){
		if(nullptr==d){
			d=new QDebugOutput;
		}
		return *d;
	}

	static QDebugOutput &warning(){
		if(nullptr==w){
			w=new QDebugOutput;
		}
		return *w;
	}

};

QDebugOutput &operator<<(QDebugOutput &d, const char *str);

#define qDebug QDebugOutput::debug
#define qWarning QDebugOutput::warning


#else
#include <inttypes.h>
#include <cstring>
#include <iostream>
using namespace std;
#include <QtGlobal>
#include <QDebug>

#endif


#endif // ARDUMY_HPP
