#include "System.h"
#include "Common.h"

#include <fstream>
#include <sstream>
#include <unistd.h>
#include <iostream>

System::System(const std::chrono::milliseconds quantum, const std::string& name) : Task(quantum, name)
{}

SystemMemory::SystemMemory(const std::chrono::milliseconds quantum, const std::string& name) : System(quantum, name)
{}

//! @brief Returns the memory usage for the current running process
std::optional<SystemMemory::MemoryInfo> SystemMemory::GetMemoryUsage(pid_t pid) 
{
    MemoryInfo mem_info;
    std::ifstream file("/proc/" + std::to_string(pid) + "/status");

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        long value;
        std::string unit;

        iss >> key >> value >> unit;
        if (key == "VmRSS:") {
            mem_info.vmrss_kb = value;
            file.close();
            return mem_info;
        }
    }
    return std::nullopt;
}

//! @brief Execute this class behaviour
bool SystemMemory::execute(std::atomic<bool>& should_stop)
{
    // Check if the task should return execution
    if (should_stop.load()) {
        return false;
    }

    start_.push_back(std::chrono::steady_clock::now());

    pid_t pid = getpid();
    std::optional<SystemMemory::MemoryInfo> total_mem = GetMemoryUsage(pid);
    if (!total_mem)
    {
        // Task did not complete
        INFO_ERR("Failed to retrieve memory usage for PID: %d\n", pid);
        return false;
    }
    INFO_LOG("Total Memory Usage (RSS): %ldKB\n", (*total_mem).vmrss_kb);

    end_.push_back(std::chrono::steady_clock::now());

    // Work out the length the task took
    CalculateMetrics();

    return true;
}

//! @brief Create a new instance of a System
std::unique_ptr<SystemMemory> SystemBuilder::Create(System::ProbeType type, const std::chrono::milliseconds quantum, const std::string& name)
{
    switch(type)
    {
        case System::ProbeType::MEMORY:
            return std::make_unique<SystemMemory>(quantum, name);
        default:
            return nullptr;
    }
}

