
#include <getopt.h>
#include <string>
#include <chrono>
#include <future>

#include "Common.h"
#include "Network.h"
#include "Process.h"
#include "PeriodicScheduler.h"

// This is an implementation of a periodic scheduler
// The same tasks added to the task list are re-run every N seconds and aggregate results are collected.

//! Maintain a fixed quantum for each task, RR implementation
constexpr std::chrono::milliseconds kTimeQuantum = std::chrono::milliseconds(10);

//! @brief Adds a task to the list the scheduler should manage
void Scheduler::AddTask(std::unique_ptr<Task> task, uint32_t interval, const std::string &name)
{
    ScheduledTask t;
    t.name = name;
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
                bool complete = t.task->execute();
                t.last_execution = now;

                if (complete) {
                    PrintMetrics(t);
                }
                else
                {
                    INFO_LOG("Rescheduled task: %s\n", t.name.c_str());
                }
            }
            // Sleep the main thread for a period
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

//! @brief Print the ongoing metrics for this task
void Scheduler::PrintMetrics(const ScheduledTask& task) const
{
    INFO_LOG("Task statistics %s\n", task.name.c_str());
    INFO_LOG("  Min time: %d ms\n", task.task->GetMetrics().min);
    INFO_LOG("  Max time: %d ms\n", task.task->GetMetrics().max);
    INFO_LOG("  Avg time: %d ms\n", task.task->GetMetrics().avg);
}

int main(int argc, char **argv)
{
    const char* const short_opts = "n:s:";
    const option long_opts[] = {
        {"network_interval", required_argument, nullptr, 'n'},
        {"system_interval", required_argument, nullptr, 's'},
        {nullptr, 0, nullptr, 0}
    };

    int32_t network_interval = 1500;
    int32_t system_interval = 2000;

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

    // Create some tasks
    NetworkBuilder network_builder;
    scheduler.AddTask(std::move(network_builder.Create("booth.pm", kTimeQuantum)), network_interval, "booth.pm");
    scheduler.AddTask(std::move(network_builder.Create("google.com", kTimeQuantum)), network_interval, "google.com");
    scheduler.AddTask(std::move(network_builder.Create("cisco.com", kTimeQuantum)), network_interval, "cisco.com");

    scheduler.Run();
    return 0;
}