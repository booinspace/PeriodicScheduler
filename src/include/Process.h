#ifndef PROCESS_H
#define PROCESS_H

#include <string>

#include "Task.h"


class Process : public Task
{
public:
    Process();
    bool execute() override;
};

#endif