

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <uiohook.h>

#include "logger.h"

static bool default_logger(unsigned int level, const char *format, ...) {
    return false;
}

// Current logger function pointer, should never be null.
logger_t logger = &default_logger;

UIOHOOK_API void hook_set_logger_proc(logger_t logger_proc) {
    if (logger_proc == NULL) {
        logger = &default_logger;
    } else {
        logger = logger_proc;
    }
}
