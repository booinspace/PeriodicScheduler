#ifndef NETWORK_h
#define NETWORK_H

#include <string>
#include <deque>

#include "Task.h"

class Network : public Task
{
public:
    Network(const std::string& domain);
    void execute() override;
    int32_t ResolveDomain(const std::string& domain);
private:
    std::deque<int32_t> resolve_times_;
    std::string domain_;
};

#endif