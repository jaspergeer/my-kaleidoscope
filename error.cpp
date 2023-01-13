#include "error.h"
#include <iostream>

void logError(const char *str) {
    fprintf(stderr, "LogError: %s\n", str);
}