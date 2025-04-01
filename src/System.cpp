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

struct MemoryInfo {
    long vmrss_kb = 0; // Resident Set Size (actual memory used)
    long vmsize_kb = 0; // Virtual memory size
};

MemoryInfo getMemoryUsage(pid_t pid) {
    MemoryInfo memInfo;
    std::ifstream file("/proc/" + std::to_string(pid) + "/status");

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        long value;
        std::string unit;

        iss >> key >> value >> unit;
        if (key == "VmRSS:") {
            memInfo.vmrss_kb = value;  // Resident Set Size
        } else if (key == "VmSize:") {
            memInfo.vmsize_kb = value;  // Virtual Memory Size
        }
    }
    return memInfo;
}

bool SystemMemory::execute()
{
    pid_t pid = getpid();
    MemoryInfo totalMem = getMemoryUsage(pid);
    std::cout << "Total Memory Usage (RSS): " << totalMem.vmrss_kb << " KB\n";
    std::cout << "Total Virtual Memory: " << totalMem.vmsize_kb << " KB\n";

    return true;
}

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

