#include "Task.h"
#include "Network.h"
#include "Process.h"
#include "Common.h"

Task::Task(const std::string& name, uint32_t frequency) :
    name_(name),
    frequency_(frequency)
{
    STATUS_LOG("Task::Task Called");
}

const std::string Task::GetName(void) const
{
    return name_;
}

uint32_t Task::GetFrequency(void) const
{
    return frequency_;
}