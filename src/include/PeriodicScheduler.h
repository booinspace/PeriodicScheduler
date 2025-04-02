#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <string>
#include <vector>
#include <thread>
#include "Task.h"

//! Define a Task to run
struct ScheduledTask
{
    //! Name of the task for logging
    std::string name;
    //! How often this task should run
    uint32_t interval;
    //! A time point for when this task was last run
    std::chrono::steady_clock::time_point last_execution;
    //! The task object itself
    std::unique_ptr<Task> task;
};

class Scheduler
{
public:
    //! Default constructors and destructors
    Scheduler() = default;
    ~Scheduler() = default;

    // Delete the Copy and Move constructors
    Scheduler(const Scheduler& scheduler) = delete;
    Scheduler(Scheduler &&scheduler) noexcept = delete;

    //! @brief Adds a task to the list the scheduler should manage
    //! @param[in] task The task object
    //! @param[in] interval How often the task should execute
    void AddTask(std::unique_ptr<Task> task, uint32_t interval);

    //! @brief Executes tasks on the worker thread.
    //! If any task takes longer than the quantum
    //! the thread will yield and the next task is worked on
    void Run(void);

    //! @brief Print the ongoing metrics for this task
    //! @param[in] task The task to print metrics for
    void PrintMetrics(const ScheduledTask& task) const;

private:
    // The scheduler thread
    std::thread thread_;

    // Atomic boolean used if the task exceeds quantum
    std::atomic<bool> should_stop_ = false;

    // The list of tasks that should be periodically scheduled
    std::vector<ScheduledTask> tasks_;

    // Used to indicate if the scheduler should stop
    bool running_ = true;
};

#endif