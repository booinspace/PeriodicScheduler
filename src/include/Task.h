#ifndef TASK_H
#define TASK_H

#include <string>
#include <memory>
#include <stdint.h>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

struct TaskMetrics
{
    int32_t min;
    int32_t avg;
    int32_t max;
};

class Task
{
public:
    Task();
    virtual ~Task() = default;

    virtual bool execute() = 0;
    TaskMetrics GetMetrics(void) const;

protected:
    TaskMetrics metrics_;
    std::mutex mtx_;
};

#endif