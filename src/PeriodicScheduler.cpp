
#include <getopt.h>
#include <string>
#include <chrono>
#include <future>

#include "Common.h"
#include "Network.h"
#include "System.h"
#include "PeriodicScheduler.h"

// This is an implementation of a periodic scheduler
// The same tasks added to the task list are re-run every N seconds and aggregate results are collected.
// Round-robin is used to ensure tasks don't exceed a set quantum value and ensure fairness for all tasks in the scheduler

//! Maximum time quantum for a task before yielding execution to the scheduler
constexpr std::chrono::milliseconds kTimeQuantum = std::chrono::milliseconds(500);

//! @brief Adds a task to the list the scheduler should manage
void Scheduler::AddTask(std::unique_ptr<Task> task, uint32_t interval)
{
    ScheduledTask t;
    t.interval = interval;
    t.last_execution = std::chrono::steady_clock::now();
    t.task = std::move(task);

    tasks_.push_back(std::move(t));
}

//! @brief Executes tasks on the worker thread.
//! If any task takes longer than the quantum
//! the thread will yield and the next task is worked on
void Scheduler::Run(void)
{
    while(running_)
    {

        const auto now = std::chrono::steady_clock::now();
        for (ScheduledTask &t : tasks_ )
        {
            if (now >= (t.last_execution + std::chrono::milliseconds(t.interval)))
            {
                INFO_LOG("Running task: %s\n", t.task->GetName().c_str());
                // Run the task asynchronously
                auto future = std::async(std::launch::async, [&]() {
                    return t.task->execute(should_stop_);
                });

                // Wait for the task to complete within the quantum period
                if (future.wait_for(kTimeQuantum) == std::future_status::timeout)
                {
                    // Task exceeded its quantum, reschedule it
                    should_stop_.store(true);
                    INFO_LOG("Task %s exceeded quantum and was rescheduled\n", t.task->GetName().c_str());
                }
                else
                {
                    should_stop_.store(false);

                    // Task completed within the quantum
                    bool complete = future.get();
                    t.last_execution = now;

                    if (complete)
                    {
                        PrintMetrics(t);
                    }
                }
            }
            // Sleep the main thread for a period
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

//! @brief Print the ongoing metrics for this task
void Scheduler::PrintMetrics(const ScheduledTask& t) const
{
    INFO_LOG("Task statistics %s\n", t.task->GetName().c_str());
    INFO_LOG("Min time: %.2f ms\n", t.task->GetMetrics().min);
    INFO_LOG("Max time: %.2f ms\n", t.task->GetMetrics().max);
    INFO_LOG("Avg time: %.2f ms\n\n", t.task->GetMetrics().avg);
}

int main(int argc, char **argv)
{
    const char* const short_opts = "n:s:";
    const option long_opts[] = {
        {"network_interval", required_argument, nullptr, 'n'},
        {"system_interval", required_argument, nullptr, 's'},
        {nullptr, 0, nullptr, 0}
    };

    // Intervals in milliseconds
    int32_t network_interval = 4000;
    int32_t system_interval = 3000;

    int opt;
    while ((opt = getopt_long(argc, argv, short_opts, long_opts, nullptr)) != -1)
    {
        switch (opt)
        {
            case 'n':
                network_interval = std::stoi(optarg);
                break;
            case 's':
                system_interval = std::stoi(optarg);
                break;
        }
    }

    // Instanciate the scheduler
    Scheduler scheduler;

    // Create some network tasks
    NetworkBuilder network_builder;
    scheduler.AddTask(std::move(network_builder.Create("google.com", kTimeQuantum)), network_interval);
    scheduler.AddTask(std::move(network_builder.Create("cisco.com", kTimeQuantum)), network_interval);

    // Create some system tasks
    SystemBuilder system_builder;
    scheduler.AddTask(std::move(system_builder.Create(System::ProbeType::MEMORY, kTimeQuantum, "MEMORY")), system_interval);

    scheduler.Run();
    return 0;
}