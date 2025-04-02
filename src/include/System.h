#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <optional>

#include "Task.h"

class System : public Task
{
public:
    //! Type of system probe
    enum ProbeType
    {
        SWAP = 0,
        MEMORY = 1,
        PACKETS = 2
    };

    System(const std::chrono::milliseconds quantum, const std::string& name);
    bool execute(std::atomic<bool>& should_stop) = 0;
};

class SystemMemory : public System
{
public:
    struct MemoryInfo {
        // Resident Set Size (actual memory used in physical memory)
        long vmrss_kb;
    };

    SystemMemory(const std::chrono::milliseconds quantum, const std::string& name);
    bool execute(std::atomic<bool>& should_stop) override;

private:
    //! @brief Get the memory usage of the current pid
    std::optional<MemoryInfo> GetMemoryUsage(pid_t pid);
};

class SystemBuilder
{
public:
    SystemBuilder() = default;
    ~SystemBuilder() = default;

    // Delete copy constructor
    SystemBuilder(const SystemBuilder& other) = delete;
    // Delete copy assignment constructor
    SystemBuilder& operator=(const SystemBuilder& other) = delete;
    // Delete move constructor
    SystemBuilder(SystemBuilder&& other) noexcept = delete;
    // Delete move assignment constructor
    SystemBuilder& operator=(SystemBuilder&& other) noexcept = delete;

    //! @brief Create a new instance of a System
    //! @return A unique pointer to a newly created System class
    std::unique_ptr<SystemMemory> Create(System::ProbeType type, const std::chrono::milliseconds quantum, const std::string& name);
};

#endif