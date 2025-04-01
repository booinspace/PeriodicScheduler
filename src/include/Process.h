#ifndef PROCESS_H
#define PROCESS_H

#include <string>

#include "Task.h"


class Process : public Task
{
public:
    Process(const std::chrono::milliseconds quantum);
    bool execute() override;
};

#endif