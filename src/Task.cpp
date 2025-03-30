#include "Task.h"
#include "Network.h"
#include "Process.h"
#include "Common.h"

Task::Task(uint32_t frequency) :
    frequency_(frequency),
    last_execution_(std::chrono::steady_clock::now()),
    running_(true)
{
    STATUS_LOG("Task::Task");

    // Create a thread which executes work for this task
    worker_thread_ = std::thread([&] {
        std::unique_lock<std::mutex> lock(mtx_);
        while(running_)
        {
            cv_.wait_for(lock, std::chrono::milliseconds(frequency_), [this] { return !running_; });
            if (!running_) break;
            execute();
        }
    });
}

uint32_t Task::GetFrequency(void) const
{
    return frequency_;
}

std::chrono::steady_clock::time_point Task::GetLastExecution() const
{
    return last_execution_;
}

void Task::UpdateLastExecution(std::chrono::steady_clock::time_point t)
{
    last_execution_ = t;
}

void Task::Notify(void)
{
    cv_.notify_one();
}