#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>

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
    bool execute() = 0;
};

class SystemMemory : public System
{
public:
    SystemMemory(const std::chrono::milliseconds quantum, const std::string& name);
    bool execute() override;
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

    //! @brief Create a new instance of a System class
    //! @return A unique pointer to a newly created System class
    std::unique_ptr<SystemMemory> Create(System::ProbeType type, const std::chrono::milliseconds quantum, const std::string& name);
};

#endif