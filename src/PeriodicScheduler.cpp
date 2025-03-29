
#include <getopt.h>
#include <string>
#include <chrono>

#include "Common.h"
#include "Network.h"
#include "Process.h"
#include "PeriodicScheduler.h"

const std::unique_ptr<Task> Scheduler::create(TaskTypes t, const std::string& name, uint32_t frequency)
{
    switch(t)
    {
        case TaskTypes::NETWORK:
            return std::make_unique<Network>(name, frequency);
        case TaskTypes::PROCESS:
            return std::make_unique<Process>(name, frequency);
        default:
            return nullptr;  
    }
}

void Scheduler::AddTask(std::unique_ptr<Task> task)
{
    tasks_.push_back(std::move(task));
}

void Scheduler::Run()
{
    while(running_)
    {
        for (const std::unique_ptr<Task> &t : tasks_ )
        {
            t->execute();
        }
    }
}

int main(int argc, char **argv)
{
    const char* const short_opts = "n:s:";
    const option long_opts[] = {
        {"network_interval", required_argument, nullptr, 'n'},
        {"system_interval", required_argument, nullptr, 's'},
        {nullptr, 0, nullptr, 0}
    };

    int32_t network_interval = 1;
    int32_t system_interval = 1;

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

    // Create some tasks
    Scheduler scheduler;

    scheduler.AddTask(scheduler.create(TaskTypes::NETWORK, "network", network_interval));
    scheduler.AddTask(scheduler.create(TaskTypes::PROCESS, "process", system_interval));


    return 0;
}