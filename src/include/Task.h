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
    Task(uint32_t frequency);
    virtual ~Task() = default;

    virtual void execute() = 0;

    const std::string GetName(void) const;

    uint32_t GetFrequency(void) const;

    std::chrono::steady_clock::time_point GetLastExecution(void) const;

    void UpdateLastExecution(std::chrono::steady_clock::time_point t);

    void Notify(void);

protected:
    uint32_t frequency_;
    std::chrono::steady_clock::time_point last_execution_;
    std::thread worker_thread_;
    std::mutex mtx_;
    std::condition_variable cv_;
    bool running_;
};

#endif