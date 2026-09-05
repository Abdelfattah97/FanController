#pragma once

#include <Arduino.h>

#ifdef DEBUG_FEATURE
void debugPrint(const char *message);
void debugPrint(const String &message);
void debugPrint(int value);
void debugPrintln();
void debugPrintln(const char *message);
void debugPrintln(const String &message);
void debugPrintln(int value);
void debugPrintf(const char *format, ...);
void debugRemoteCommand(uint16_t address, uint8_t command, bool accepted);
#else
inline void debugPrint(const char *) {}
inline void debugPrint(const String &) {}
inline void debugPrint(int) {}
inline void debugPrintln() {}
inline void debugPrintln(const char *) {}
inline void debugPrintln(const String &) {}
inline void debugPrintln(int) {}
inline void debugPrintf(const char *, ...) {}
inline void debugRemoteCommand(uint16_t, uint8_t, bool) {}
#endif
