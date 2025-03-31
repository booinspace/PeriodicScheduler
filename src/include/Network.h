#ifndef NETWORK_h
#define NETWORK_H

#include <string>
#include <deque>
#include <optional>
#include <vector>

#include "Task.h"

class Network : public Task
{
public:
    Network(const std::string& domain);
    ~Network(void);

    //! @brief Execute this class behaviour
    bool execute() override;

    //! @brief Resolve a domain name to an IPV4 address
    //! NOTE: Async function, waits 100ms for domain to resolve, otherwise returns
    int ResolveDomainAsync(const std::string& domain, char* ip, size_t ip_size);
private:
    const timespec CalculateTimespec(std::chrono::milliseconds duration);

    std::vector<std::chrono::steady_clock::time_point> start_;
    std::vector<std::chrono::steady_clock::time_point> end_;
    std::deque<int32_t> resolve_times_;
    std::string domain_;
    struct gaicb* host_;
};

#endif