#pragma once

#ifdef __cplusplus
#include <Arduino.h>

#if defined(ARDUINO_ARCH_ESP32)
// Provide backward-compatible overload expected by libraries that pass the pin.
template <typename PinT>
inline void analogWriteResolution(PinT, uint8_t bits) {
    ::analogWriteResolution(bits);
}
#endif

#endif
