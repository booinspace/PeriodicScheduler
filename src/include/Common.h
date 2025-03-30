#ifndef COMMON_H
#define COMMON_H

#include <cstdint>
#include <stdio.h>

#ifdef DEBUG
#define STATUS_LOG(...) fprintf(stdout, "STATUS: " __VA_ARGS__)
#define STATUS_ERR(...) fprintf(stderr, "ERR: " __VA_ARGS__)
#else
#define STATUS_LOG(...)
#define STATUS_ERR(...)
#endif

#endif