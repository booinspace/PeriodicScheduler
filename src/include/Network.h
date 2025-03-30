#ifndef NETWORK_h
#define NETWORK_H

#include <string>
#include <vector>

#include "Task.h"

class Network : public Task
{
public:
    Network();
    void execute() override;
    int32_t ResolveDomain(const std::string& fqdn);
private:
    std::vector<int32_t> resolve_times_;
};

#endif