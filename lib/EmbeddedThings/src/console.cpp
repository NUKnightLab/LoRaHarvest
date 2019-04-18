#include <stdarg.h>

#define DEFAULT_OUTPUT_ENABLED true
static bool output_enabled = DEFAULT_OUTPUT_ENABLED;

#ifdef ARDUINO
    #include <Arduino.h>
    #define LOGGING_BUFFER_SIZE 128
    static char buf[LOGGING_BUFFER_SIZE];
    void print(const char *fmt, ... ) {
        if (output_enabled) {
            va_list args;
            va_start (args, fmt);
            vsnprintf(buf, LOGGING_BUFFER_SIZE, fmt, args);
            va_end(args);
            Serial.print(buf);
        }
    }
    void println(const char *fmt, ...) {
        if (output_enabled) {
            va_list args;
            va_start (args, fmt);
            vsnprintf(buf, LOGGING_BUFFER_SIZE, fmt, args);
            va_end(args);
            Serial.print(buf);
            Serial.println("");
        }
    }
#else
    #include <stdio.h>
    void print(const char *fmt, ... ) {
        if (output_enabled) {
            va_list args;
            va_start (args, fmt);
            printf(fmt, args);
            va_end(args);
        }
    }
    #include <iostream>
    void println(const char *fmt, ... ) {
        if (output_enabled) {
            va_list args;
            va_start (args, fmt);
            vprintf(fmt, args);
            va_end(args);
            puts("\n");
        }
    }
#endif