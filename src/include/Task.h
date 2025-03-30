#ifndef TASK_H
#define TASK_H

#include <string>
#include <memory>
#include <stdint.h>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

enum TaskTypes : uint8_t
{
    NETWORK = 0,
    PROCESS = 1
};

class Task
{
public:
    Task();
    virtual ~Task() = default;

    virtual void execute() = 0;
};

#endif