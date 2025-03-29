#ifndef TASK_H
#define TASK_H

#include <string>
#include <memory>
#include <stdint.h>

enum TaskTypes : uint8_t
{
    NETWORK = 0,
    PROCESS = 1
};

class Task
{
public:
    Task(const std::string &name, uint32_t frequency);
    virtual ~Task() = default;

    virtual void execute() = 0;

    const std::string GetName(void) const;
    uint32_t GetFrequency(void) const;
private:
    std::string name_;
    uint32_t frequency_;
};

#endif