
#include <getopt.h>
#include <string>
#include <chrono>

#include "Common.h"
#include "Network.h"
#include "Process.h"
#include "PeriodicScheduler.h"

void Scheduler::AddTask(std::unique_ptr<Task> task, uint32_t interval, const std::string &name)
{
    ScheduledTask t;
    t.name = name;
    t.interval = interval;
    t.last_execution = std::chrono::steady_clock::now();
    t.task = std::move(task);

    tasks_.push_back(std::move(t));
}

void Scheduler::Run()
{
    while(running_)
    {

        const auto now = std::chrono::steady_clock::now();
        for (ScheduledTask &t : tasks_ )
        {
            if (now >= (t.last_execution + std::chrono::milliseconds(t.interval)))
            {
                // Launch a thread to work on each task
                std::thread([&]() {
                    t.task->execute();
                    t.last_execution = now;
                    PrintMetrics(t);
                }).detach();
            }
            // Sleep the main thread for a period
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void Scheduler::PrintMetrics(const ScheduledTask& task) const
{
    STATUS_LOG("Task statistics: %s\n", task.name.c_str());
    STATUS_LOG("  Min time: %d ms\n", task.task->GetMetrics().min);
    STATUS_LOG("  Max time: %d ms\n", task.task->GetMetrics().max);
    STATUS_LOG("  Avg time: %d ms\n", task.task->GetMetrics().avg);
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
    std::unique_ptr<Task> task1 = std::make_unique<Network>("google.com");
    std::unique_ptr<Task> task2 = std::make_unique<Network>("cisco.com");
    scheduler.AddTask(std::move(task1), network_interval, "google.com");
    scheduler.AddTask(std::move(task2), network_interval, "cisco.com");


    //scheduler.AddTask(scheduler.create(TaskTypes::NETWORK), network_interval, "DNS");
    //scheduler.AddTask(scheduler.create(TaskTypes::PROCESS), system_interval, "Memory");

    scheduler.Run();
    return 0;
}