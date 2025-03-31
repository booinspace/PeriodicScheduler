#ifndef NETWORK_h
#define NETWORK_H

#include <string>
#include <deque>
#include <optional>

#include "Task.h"

class Network : public Task
{
public:
    Network(const std::string& domain);
    ~Network(void);

    //! @brief Execute this class behaviour
    void execute() override;

    //! @brief Resolve a domain name to an IPV4 address
    //! NOTE: Async function, waits 100ms for domain to resolve, otherwise returns
    int ResolveDomainAsync(const std::string& domain, char* ip, size_t ip_size);
private:
    std::optional<std::chrono::steady_clock::time_point> start_;
    std::optional<std::chrono::steady_clock::time_point> end_;
    std::deque<int32_t> resolve_times_;
    std::string domain_;
    struct gaicb* host_;
};

#endif