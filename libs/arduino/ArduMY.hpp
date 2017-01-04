#ifndef ARDUMY_HPP
#define ARDUMY_HPP


// Allow inclusion of  sources without dependency on Arduino

#ifdef ARDUINO_ARCH_AVR
#include <Arduino.h>
#else
#include <inttypes.h>
#endif


#endif // ARDUMY_HPP
