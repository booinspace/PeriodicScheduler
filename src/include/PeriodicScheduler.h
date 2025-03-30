#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <string>
#include <vector>
#include <thread>
#include "Task.h"

enum TaskTypes : uint8_t
{
    NETWORK = 0,
    PROCESS = 1
};

struct ScheduledTask
{
    std::string name;
    uint32_t interval;
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

    const std::unique_ptr<Task> create(TaskTypes t);

    void AddTask(std::unique_ptr<Task> task, uint32_t interval, const std::string &name);

    void Run();

private:
    std::thread thread_;
    std::vector<ScheduledTask> tasks_;
    bool running_ = true;
};

#endif