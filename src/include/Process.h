#ifndef PROCESS_H
#define PROCESS_H

#include <string>

#include "Task.h"


class Process : public Task
{
public:
    Process(const std::string &name, uint32_t frequency); 
    void execute() override;

    uint32_t ResolveDomain(const std::string& fqdn);
};

#endif