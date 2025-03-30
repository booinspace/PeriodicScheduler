#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "Task.h"

struct ScheduledTask
{
    std::unique_ptr<Task> task;
    std::chrono::steady_clock::time_point last_execution;
};

class Scheduler
{
public:
    Scheduler() = default;
    ~Scheduler() = default;

    // Delete the Copy and Move constructors
    Scheduler(const Scheduler& scheduler) = delete;
    Scheduler(Scheduler &&scheduler) noexcept = delete;

    const std::unique_ptr<Task> create(TaskTypes t, uint32_t frequency);
    void AddTask(std::unique_ptr<Task> task);
    void Run();

private:
    std::thread thread_;
    std::vector<std::unique_ptr<Task>> tasks_;
    bool running_ = true;
    std::mutex mutex_;
    std::condition_variable cv_;
};

#endif