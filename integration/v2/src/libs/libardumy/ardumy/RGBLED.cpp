#include "RGBLED.hpp"

// Only build when we are targeting actual Arduino hardware
#ifdef OCTOMY_ARDUINO_TARGET

const float RGBLED::amp[3] { 1.0f , 0.55f , 0.4f };

#endif // OCTOMY_ARDUINO_TARGET
