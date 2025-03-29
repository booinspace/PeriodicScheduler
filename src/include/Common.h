#ifndef COMMON_H
#define COMMON_H

#include <cstdint>
#include <stdio.h>

#define STATUS_LOG(...) fprintf(stdout, "STATUS: " __VA_ARGS__ "\n")
#define STATUS_ERR(...) fprintf(stderr, "ERR: " __VA_ARGS__ "\n")

#endif