#ifndef TASK_H
#define TASK_H

#include <string>
#include <memory>
#include <stdint.h>
#include <chrono>
#include <mutex>
#include <deque>
#include <vector>
#include <thread>
#include <atomic>

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
    Task(const std::chrono::milliseconds quantum, const std::string& name);
    virtual ~Task() = default;

    //! @brief The main task handler
    //! NOTE: This function must implement cooperative multitasking so that it yields safely after the quantum time
    virtual bool execute(std::atomic<bool>& should_stop) = 0;

    //! @brief Get the task metrics
    //! @return Metrics of the task
    TaskMetrics GetMetrics(void) const;

    //! @brief Get the name of the task
    //! @return A string containing the name of the task
    const std::string& GetName();

protected:
    void CalculateMetrics();

    //! The maximum time this task should execute for, before yielding to the scheduler
    std::chrono::milliseconds quantum_;

    //! Capture the start times for every execution of this task
    std::vector<std::chrono::steady_clock::time_point> start_;

    //! Capture the end times for every execution of this task
    std::vector<std::chrono::steady_clock::time_point> end_;

    //! Name of the task
    std::string name_;
private:
    void UpdateMetrics(TaskMetrics& metrics);

    TaskMetrics metrics_;
    std::deque<int32_t> task_times_;
};

#endif