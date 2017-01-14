#ifndef ARDUMY_HPP
#define ARDUMY_HPP


// Allow inclusion of  sources without dependency on Arduino

#ifdef ARDUINO_ARCH_AVR
#include <Arduino.h>
#define Q_ASSERT(A)
#else
#include <inttypes.h>
#include <cstring>
#include <iostream>
using namespace std;
#include <QtGlobal>

#endif


#endif // ARDUMY_HPP
