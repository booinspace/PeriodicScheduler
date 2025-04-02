#ifndef COMMON_H
#define COMMON_H

#include <cstdint>
#include <stdio.h>

// Define a debug logging macro that only prints in debug
#ifdef DEBUG
#define STATUS_LOG(...) fprintf(stdout, "STATUS: " __VA_ARGS__)
#define STATUS_ERR(...) fprintf(stderr, "STATUS_ERR: " __VA_ARGS__)
#else
#define STATUS_LOG(...)
#define STATUS_ERR(...)
#endif

// Define a logging macro that always prints in release
#define INFO_LOG(...) fprintf(stdout, "INFO: " __VA_ARGS__)
#define INFO_ERR(...) fprintf(stderr, "INFO_ERR: " __VA_ARGS__)

#endif