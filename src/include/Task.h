#ifndef TASK_H
#define TASK_H

#include <string>
#include <memory>
#include <stdint.h>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

//! Define a structure used for capturing task data
struct TaskMetrics
{
    //! Minimum time the task took
    int32_t min;
    //! Average time the task took (mean)
    int32_t avg;
    //! Maximum time the task took
    int32_t max;
};

class Task
{
public:
    //! @brief Constructor for a Task
    //! @param[in] quantum The maximum time the task should execute for
    Task(const std::chrono::milliseconds quantum);
    virtual ~Task() = default;

    //! @brief The main task handler
    //! NOTE: This function must implement cooperative multitasking so that it yields safely after the quantum time
    virtual bool execute() = 0;

    //! @brief Get the task metrics
    //! @return Metrics of the task
    TaskMetrics GetMetrics(void) const;

protected:
    //! The task metrics
    TaskMetrics metrics_;

    //! Mutex around the metrics
    std::mutex metrics_mutex_;

    //! The maximum time this task should execute for, before yielding to the scheduler
    std::chrono::milliseconds quantum_;
};

#endif